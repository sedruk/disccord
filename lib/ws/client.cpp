#include <chrono>
#include <thread>
#include <functional>

#include <disccord/rest.hpp>
#include <disccord/ws/client.hpp>

#include <disccord/ws/opcode.hpp>

#include <disccord/models/message.hpp>
#include <disccord/models/guild.hpp>

#include <disccord/ws/models/hello.hpp>
#include <disccord/ws/models/ready.hpp>
#include <disccord/ws/models/partial.hpp>

namespace disccord
{
    namespace ws
    {
        static const web::uri base_uri(DISCORD_API_BASE_URL);

        discord_ws_client::discord_ws_client(std::string token, disccord::token_type type)
            : rest_api_client(base_uri, token, type), ws_api_client(rest_api_client, token, type),
            dispatcher(), state(5000), session_id(""), sequence(0), heartbeat_count(0),
            heartbeat_cancel_token(), heartbeat_task()
        {
            ws_api_client.set_frame_handler([this](const disccord::ws::models::frame* frame)
            {
                return this->handle_frame(frame);
            });
            
            map_events();
        }

        discord_ws_client::~discord_ws_client()
        { }

        pplx::task<void> discord_ws_client::connect(const pplx::cancellation_token& token)
        {
            return ws_api_client.connect(token);
        }

        void discord_ws_client::map_events()
        {
            //TODO: make these event strings defined constants
            events = {
                {"UNKNOWN", event::UNKNOWN}, // if unhandled string is pass, map will default to this key-value
                {"READY", event::READY},
                {"MESSAGE_CREATE", event::MESSAGE_CREATE},
                {"MESSAGE_DELETE", event::MESSAGE_DELETE},
                {"GUILD_CREATE", event::GUILD_CREATE}
            };
        }
        
        unsigned int discord_ws_client::event(event::event ev, std::function<void ()> func)
        {
            return dispatcher.on(ev, func);
        }
        
        pplx::task<void> discord_ws_client::handle_frame(const disccord::ws::models::frame* frame)
        {
            switch (frame->get_opcode())
            {
                case opcode::HELLO:
                {
                    auto d = frame->get_data<models::hello>();
                    
                    std::cout << "Interval: " << d.get_heartbeat_interval() << '\n';
                    
                    auto func = std::bind(&discord_ws_client::heartbeat_loop, this, d.get_heartbeat_interval());
                    heartbeat_task = pplx::create_task(func, pplx::task_options(heartbeat_cancel_token.get_token()));
                    
                    std::cout << "Sending Identify.\n";
                    identify().wait();
                    break;
                }
                case opcode::HEARTBEAT_ACK:
                    std::cout << "Heartbeat acknowledged\n--------------\n";
                    break;
                case opcode::DISPATCH:
                    handle_dispatch(frame);
                    break;
                case opcode::INVALIDATE_SESSION:
                    break;
                case opcode::RECONNECT:
                    break;
                default:
                    std::cout << "Unhandled opcode " << frame->get_opcode() << std::endl;
            }

            return pplx::create_task([](){});
        }

        void discord_ws_client::handle_dispatch(const disccord::ws::models::frame* frame)
        {
            auto s = frame->get_sequence().get_value();
            auto t = frame->get_event().get_value();
            
            sequence = s;
            
            std::cout << "Received event: " << t << "\n";
            
            // this returns a correctly mapped enum or the first val, 0 (by nature of std::unordered_map), in cases when
            // there is a new/unknown event sent from discord, so we need our own default case to handle it properly
            switch(events[t])
            {
                case event::READY:
                {
                    auto rdy = frame->get_data<models::ready>();
                    session_id = rdy.get_session_id();
                    state.process_ready(rdy);
                    dispatcher.emit(event::READY);
                    break;
                }
                case event::GUILD_CREATE:
                {
                    auto gld = frame->get_data<disccord::models::guild>();
                    state.process_guild_create(gld);
                    dispatcher.emit(event::GUILD_CREATE, gld);
                    break;
                }
                case event::MESSAGE_CREATE:
                {
                    auto msg = frame->get_data<disccord::models::message>();
                    dispatcher.emit(event::MESSAGE_CREATE, msg);
                    state.process_message_create(msg);
                    break;
                }
                case event::MESSAGE_DELETE:
                {
                    auto partial_msg = frame->get_data<models::partial>();
                    auto msg = state.process_message_delete(partial_msg);
                    dispatcher.emit(event::MESSAGE_DELETE, msg);
                    break;
                }
                case event::UNKNOWN:
                {
                    break;
                }
                default:
                {
                    
                }
            }
        }
        
        pplx::task<void> discord_ws_client::heartbeat_loop(int wait_ms)
        {
            return pplx::create_task([wait_ms,this](){
                while(!pplx::is_task_cancellation_requested())
                {
                    heartbeat_count++;
                    
                    web::json::value frame;
                    frame["op"] = web::json::value(opcode::HEARTBEAT);
                    if (!sequence)
                        frame["d"] = web::json::value::null();
                    else
                        frame["d"] = web::json::value(sequence);
                    
                    std::cout << "--------------\nSending heartbeat [" << heartbeat_count << "], sequence [" 
                              << sequence << "]\n";
                    ws_api_client.send(frame.serialize()).wait();
                    
                    std::cout << "Waiting for: " << wait_ms << "ms\n";
                    // night night
                    std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
                }
                
                pplx::cancel_current_task();
            });
        }
        
        pplx::task<void> discord_ws_client::identify()
        {
            web::json::value frame;
            frame["op"] = web::json::value(opcode::IDENTIFY);
            frame["d"]["token"] = web::json::value(ws_api_client.get_token());
            frame["d"]["properties"]["$os"] = web::json::value("linux");
            frame["d"]["properties"]["$browser"] = web::json::value("disccord");
            frame["d"]["properties"]["$device"] = web::json::value("disccord");
            frame["d"]["properties"]["$referrer"] = web::json::value("");
            frame["d"]["properties"]["$referring_domain"] = web::json::value("");
            frame["d"]["compress"] = web::json::value(false);
            frame["d"]["large_threshold"] = web::json::value(250);
            
            return ws_api_client.send(frame.serialize());
        }
    } // namespace ws
} // namespace disccord
