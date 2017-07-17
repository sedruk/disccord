#ifndef _ws_connection_hpp_
#define _ws_connection_hpp_

#include <deque>
#include <utility>
#include <unordered_map>

#include <disccord/util/optional.hpp>

#include <disccord/models/user.hpp>
#include <disccord/models/message.hpp>
#include <disccord/models/guild.hpp>

#include <disccord/ws/models/ready.hpp>
#include <disccord/ws/models/partial.hpp>

namespace disccord
{
    namespace ws
    {
        namespace internal
        {
            // process all incoming dispatches (and other events) e.g. process_ready, process_message_create, etc
            // also track things like successful resumes, client status/game,  etc
            // hold a cache for messages and users
            // track a count for each event
            class connection
            {
                public:
                    using message_cache = std::deque<disccord::models::message>;
                    using user_cache = std::unordered_map<uint64_t, disccord::models::user>;
                    using guild_cache = std::unordered_map<uint64_t, disccord::models::guild>;
                    using optional_message = disccord::util::optional<disccord::models::message>;
                    
                    connection();
                    connection(size_t messages_limit = 1000);
                    virtual ~connection();
                    
                    // Event processing and dispatch
                    void process_ready(disccord::ws::models::ready ready);
                    void process_guild_create(disccord::models::guild guild);
                    void process_message_create(disccord::models::message messsage);
                    disccord::models::message process_message_delete(disccord::ws::models::partial partial_messsage);
                    
                private:
                    size_t messages_limit_;
                    
                    disccord::models::user user_;
                    
                    message_cache messages_;
                    user_cache users_;
                    guild_cache guilds_;
                    
                    // Connection utilities
                    std::pair<optional_message, int> get_message(uint64_t id);
                    pplx::task<void> load_guilds();
                    
            };
        }
    }
}

#endif /* _ws_connection_hpp_ */
