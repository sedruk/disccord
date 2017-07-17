#include <disccord/ws/models/partial.hpp>

#include <iostream>

namespace disccord
{
    namespace ws
    {
        namespace models
        {
            partial::partial()
                : channel_id()
            { }

            partial::~partial()
            { }

            void partial::decode(web::json::value json)
            {
                entity::decode(json);
                
                #define get_id_field(var) \
                if (json.has_field(#var)) { \
                    auto field = json.at(#var); \
                    if (!field.is_null()) { \
                        var = decltype(var)(std::stoull(field.as_string())); \
                    } else { \
                        var = decltype(var)::no_value(); \
                    } \
                } else { \
                    var = decltype(var)(); \
                }
                
                get_id_field(channel_id);

                #undef get_id_field
            }

            void partial::encode_to(std::unordered_map<std::string, web::json::value> &info)
            {
                entity::encode_to(info);
                
                if (channel_id.is_specified())
                    info["channel_id"] = web::json::value(std::to_string(channel_id.get_value()));
            }

            #define define_get_method(field_name) \
            decltype(partial::field_name) partial::get_##field_name() const{ \
                return field_name; \
            }

            define_get_method(channel_id)

            #undef define_get_method
        }
    }
}
