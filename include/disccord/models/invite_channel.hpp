#ifndef _invite_channel_hpp_
#define _invite_channel_hpp_

#include <disccord/models/entity.hpp>

namespace disccord
{
    namespace models
    {
        class invite_channel : public entity
        {
            public:
                invite_channel();
                virtual ~invite_channel();

                virtual void decode(web::json::value json) override;

                std::string get_name();
                uint8_t get_type();

            protected:
                virtual void encode_to(std::unordered_map<std::string, web::json::value> &info) override;

            private:
                std::string name;
                uint8_t type;
        };
    }
}

#endif /* _invite_channel_hpp_ */
