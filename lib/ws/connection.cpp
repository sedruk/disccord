#include <thread>
#include <chrono>

#include <disccord/ws/connection.hpp>

namespace disccord
{
    namespace ws
    {
        namespace internal
        {
            connection::connection()
                : messages_limit_(1000), user_(), messages_(), users_(), guilds_()
            { }
            
            connection::connection(size_t messages_limit)
                : messages_limit_(messages_limit), user_(), messages_(), users_(), guilds_()
            { }
            
            connection::~connection()
            { }
            
            // Event processing and dispatch
            void connection::process_ready(disccord::ws::models::ready ready)
            {
                std::cout << "processing ready...\n";
                user_ = ready.get_user();
                load_guilds().wait();
                
                //sync guilds, etc?
            }
            
            void connection::process_message_create(disccord::models::message messsage)
            {
                if (messages_.size() + 1 > messages_limit_)
                {
                    messages_.pop_front();
                    messages_.push_back(messsage);
                }
                else
                    messages_.push_back(messsage);
                std::cout << "Message Cache size: " << messages_.size() << '\n';
            }
            
            disccord::models::message connection::process_message_delete(disccord::ws::models::partial partial_messsage)
            {
                auto msg_pos = get_message(partial_messsage.get_id());
                auto msg = msg_pos.first;
                auto pos = msg_pos.second;
                if (msg.is_specified())
                {
                    messages_.erase(messages_.begin() + pos);
                    std::cout << "Message Cache size: " << messages_.size() << '\n';
                    return msg.get_value();
                }
                else
                {
                    std::cout << "Message Cache size: " << messages_.size() << '\n';
                    // for now just send back a nothing message
                    disccord::models::message empty_msg;
                    return empty_msg;
                }
            }
            
            void connection::process_guild_create(disccord::models::guild guild)
            {
                std::cout << "processing guild_create...\n";
                if (guild.get_unavailable().get_value())
                    return;
                // other stuff here
                guilds_.insert({guild.get_id(), guild});
            }
            
            // Connection utilities
            std::pair<disccord::util::optional<disccord::models::message>, int> connection::get_message(uint64_t id)
            {
                auto msg = optional_message();
                int pos = 0;
                for (message_cache::iterator it = messages_.begin(); it != messages_.end(); ++it, pos++)
                {
                    if (it->get_id() == id)
                    {
                        msg = optional_message(*it);
                        break;
                    }
                }
                return std::make_pair(msg, pos);
            }
            
            pplx::task<void> connection::load_guilds()
            {
                return pplx::create_task([this](){
                    // do the stuff (wait approx 4 seconds)
                    std::this_thread::sleep_for(std::chrono::seconds(4));
                    std::cout << "processing ready done!\n";
                });
            }
        } // namespace internal
    } // namespace ws
} // namespace disccord
