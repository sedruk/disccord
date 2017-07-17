#ifndef _partial_hpp_
#define _partial_hpp_

#include <string>
#include <vector>

#include <disccord/models/entity.hpp>
#include <disccord/util/optional.hpp>

#include <disccord/models/user.hpp>
#include <disccord/models/emoji.hpp>

// This class is for capturing any of the events that only send 'partial' data,
// (e.g.) and id vs the object, back to the client. Only about 2-4 of these fields will be
// decoded for each event, then it will be sent to our connection state to see if the full
// object, from the id's, can be grabbed from cache and then emitted out to the user.
namespace disccord
{
    namespace ws
    {
        namespace models
        {
            class partial : public disccord::models::entity
            {
                public:
                    partial();
                    virtual ~partial();

                    void decode(web::json::value json) override;

                    util::optional<uint64_t> get_channel_id() const;

                protected:
                    void encode_to(std::unordered_map<std::string, web::json::value> &info) override;

                private:
                    util::optional<uint64_t> channel_id;
                    util::optional<uint64_t> guild_id;
                    util::optional<bool> unavailable;
                    util::optional<std::vector<disccord::models::emoji>> emojis;
                    util::optional<disccord::models::user> user;
                    util::optional<std::vector<uint64_t>> roles;
                    util::optional<std::string> nick;
                    //TODO: add rest of partial's
                    
            };
        }
    }
}

#endif /* _partial_hpp_ */
