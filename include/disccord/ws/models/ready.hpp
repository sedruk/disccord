#ifndef _ready_hpp_
#define _ready_hpp_

#include <string>
#include <vector>

#include <disccord/models/model.hpp>
#include <disccord/models/user.hpp>
#include <disccord/models/channel.hpp>

namespace disccord
{
    namespace ws
    {
        namespace models
        {
            class ready : public disccord::models::model
            {
                public:
                    ready();
                    virtual ~ready();

                    void decode(web::json::value json) override;

                    int get_version() const;
                    std::vector<web::json::value> get_guilds() const;
                    disccord::models::user get_user() const;
                    std::string get_session_id() const;
                    std::vector<std::string> get_trace() const;

                protected:
                    void encode_to(std::unordered_map<std::string, web::json::value> &info) override;

                private:
                    int v;
                    disccord::models::user user;
                    // private channels will now always be empty in gateway v6, and probably removed in gateway v7
                    std::vector<web::json::value> guilds;
                    std::string session_id;
                    std::vector<std::string> _trace;
                    
            };
        }
    }
}

#endif /* _ready_hpp_ */
