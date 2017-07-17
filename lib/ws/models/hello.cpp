#include <disccord/ws/models/hello.hpp>

namespace disccord
{
    namespace ws
    {
        namespace models
        {
            hello::hello()
                : heartbeat_interval(0), trace({})
            { }

            hello::~hello()
            { }

            void hello::decode(web::json::value json)
            {
                heartbeat_interval = json.at("heartbeat_interval").as_integer();
                
                if (json.has_field("_trace"))
                {
                    auto _trace_array = json.at("_trace").as_array();
                    std::vector<std::string> traces_array(_trace_array.size());
                    std::transform(_trace_array.begin(), _trace_array.end(), traces_array.begin(), [](web::json::value _trace)
                        {
                            return _trace.as_string();
                        });
                    trace = traces_array;
                }
            }

            void hello::encode_to(std::unordered_map<std::string, web::json::value> &info)
            {
                info["heartbeat_interval"] = web::json::value::number(heartbeat_interval);
                auto _trace = trace;
                std::vector<web::json::value> trace_array(_trace.size());
                std::transform(_trace.begin(), _trace.end(), trace_array.begin(), [](std::string val)
                    {
                        return web::json::value(val);
                    });
                info["_trace"] = web::json::value::array(trace_array);
            }

            int hello::get_heartbeat_interval() const
            {
                return heartbeat_interval;
            }
            std::vector<std::string> hello::get_trace() const
            {
                return trace;
            }
        }
    }
}
