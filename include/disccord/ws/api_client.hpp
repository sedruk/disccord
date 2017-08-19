#ifndef _ws_api_client_hpp_
#define _ws_api_client_hpp_

#include <string>
#include <functional>

#include <cpprest/ws_client.h>

#include <disccord/ws/opcode.hpp>

#include <disccord/types.hpp>
#include <disccord/models/game.hpp>
#include <disccord/models/ws/frame.hpp>

namespace disccord
{
    namespace rest
    {
        namespace internal
        {
            class rest_api_client;
        }
    }
    namespace ws
    {
        namespace internal
        {
            class ws_api_client
            {
                public:
                    ws_api_client(disccord::rest::internal::rest_api_client& api_client, std::string token, disccord::token_type type);
                    ws_api_client(disccord::rest::internal::rest_api_client& api_client, std::string token, disccord::token_type type, const web::websockets::client::websocket_client_config& client_config);
                    virtual ~ws_api_client();

                    pplx::task<void> connect(const pplx::cancellation_token& token = pplx::cancellation_token::none());
                    
                    pplx::task<void> send(ws::opcode op, web::json::value payload);

                    void set_frame_handler(const std::function<pplx::task<void>(const disccord::models::ws::frame*)>& func);
                    
                    pplx::task<void> send_heartbeat(const uint32_t sequence = 0);
                    
                    // TODO: have a default `presence` object
                    pplx::task<void> send_identify(const uint16_t shard_id = 0, const uint16_t shard_count = 1, const bool compress = false, const uint16_t large_threshold = 100);
                    
                    pplx::task<void> send_resume(const std::string& session_id, const uint32_t sequence);
                    
                    pplx::task<void> send_status_update(const std::string& status, const models::game game, const bool afk = false, const uint64_t since = 0);
                    
                    pplx::task<void> send_request_guild_members(const disccord::snowflake guild_id, const std::string& query = "", const uint32_t limit = 0);
                    
                    pplx::task<void> send_voice_state_update(const disccord::snowflake guild_id, const bool self_mute = false, const bool self_deaf = false, const disccord::snowflake channel_id = 0);

                private:
                    web::websockets::client::websocket_client ws_client;
                    std::string token;
                    disccord::token_type token_type;
                    disccord::rest::internal::rest_api_client& rest_api_client;
                    std::function<pplx::task<void>(const disccord::models::ws::frame*)> message_handler;

                    pplx::task<void> read_task;
                    pplx::cancellation_token_source cancel_token;

                    void read_loop();
                    pplx::task<void> handle_message(const web::websockets::client::websocket_incoming_message& message);
            };
        }
    }
}

#endif /* _ws_api_client_hpp_ */
