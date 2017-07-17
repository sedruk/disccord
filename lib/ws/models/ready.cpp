#include <disccord/ws/models/ready.hpp>

namespace disccord
{
    namespace ws
    {
        namespace models
        {
            ready::ready()
                : v(0), user(), guilds(),
                session_id(""), _trace({})
            { }

            ready::~ready()
            { }

            void ready::decode(web::json::value json)
            {
                v = json.at("v").as_integer();
                session_id = json.at("session_id").as_string();
                
                if (json.has_field("guilds"))
                {
                    auto _guilds_array = json.at("guilds").as_array();
                    std::vector<web::json::value> guilds_array(_guilds_array.size());
                    std::transform(_guilds_array.begin(), _guilds_array.end(), guilds_array.begin(), [](web::json::value _guild)
                        {
                            return _guild;
                        });
                    guilds = guilds_array;
                }
                
                if (json.has_field("user"))
                {
                    auto field = json.at("user");
                    disccord::models::user val;
                    val.decode(field);
                    user = val;
                }
                
                if (json.has_field("_trace"))
                {
                    auto _trace_array = json.at("_trace").as_array();
                    std::vector<std::string> traces_array(_trace_array.size());
                    std::transform(_trace_array.begin(), _trace_array.end(), traces_array.begin(), [](web::json::value _trace)
                        {
                            return _trace.as_string();
                        });
                    _trace = traces_array;
                }
            }

            void ready::encode_to(std::unordered_map<std::string, web::json::value> &info)
            {
                info["v"] = web::json::value(v);
                info["session_id"] = web::json::value(session_id);
                info["user"] = get_user().encode();
                
                std::vector<web::json::value> guilds_array(guilds.size());
                std::transform(guilds.begin(), guilds.end(), guilds_array.begin(), [](web::json::value val)
                    {
                        return val;
                    });
                info["guilds"] = web::json::value::array(guilds_array);
                
                std::vector<web::json::value> trace_array(_trace.size());
                std::transform(_trace.begin(), _trace.end(), trace_array.begin(), [](std::string val)
                    {
                        return web::json::value(val);
                    });
                info["_trace"] = web::json::value::array(trace_array);
            }

            int ready::get_version() const
            {
                return v;
            }
            std::vector<web::json::value> ready::get_guilds() const
            {
                return guilds;
            }
            disccord::models::user ready::get_user() const
            {
                return user;
            }
            std::string ready::get_session_id() const
            {
                return session_id;
            }
            std::vector<std::string> ready::get_trace() const
            {
                return _trace;
            }
        }
    }
}
