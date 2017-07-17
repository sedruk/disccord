#ifndef _hello_hpp_
#define _hello_hpp_

#include <string>

#include <disccord/models/model.hpp>

namespace disccord
{
    namespace ws
    {
        namespace models
        {
            class hello : public disccord::models::model
            {
                public:
                    hello();
                    virtual ~hello();

                    void decode(web::json::value json) override;

                    int get_heartbeat_interval() const;
                    std::vector<std::string> get_trace() const;

                protected:
                    void encode_to(std::unordered_map<std::string, web::json::value> &info) override;

                private:
                    int heartbeat_interval;
                    std::vector<std::string> trace;
            };
        }
    }
}

#endif /* _hello_hpp_ */
