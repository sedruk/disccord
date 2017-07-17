#include <stdexcept>

#include <disccord/ws/dispatcher.hpp>

namespace disccord
{
    namespace ws
    {
        namespace internal
        {
            dispatcher::dispatcher()
            { }

            dispatcher::~dispatcher()
            { }

            unsigned int dispatcher::add_listener(unsigned int event_id, std::function<void ()> cb)
            {
                if (!cb)
                {
                    throw std::invalid_argument("dispatcher::add_listener: No callbak provided.");
                }
                
                std::lock_guard<std::mutex> lock(mutex);

                unsigned int listener_id = ++last_listener;
                listeners.insert(std::make_pair(event_id, std::make_shared<listener<>>(listener_id, cb)));

                return listener_id;       
            }

            unsigned int dispatcher::on(unsigned int event_id, std::function<void ()> cb)
            {
                return add_listener(event_id, cb);
            }

            void dispatcher::remove_listener(unsigned int listener_id)
            {
                std::lock_guard<std::mutex> lock(mutex);

                auto i = std::find_if(listeners.begin(), listeners.end(), [&] (std::pair<const unsigned int, std::shared_ptr<listenerbase>> p) {
                    return p.second->id == listener_id;
                });
                if (i != listeners.end())
                {
                    listeners.erase(i);
                }
                else
                {
                    throw std::invalid_argument("dispatcher::remove_listener: Invalid listener id.");
                }
            }
        } // namespace internal
    } // namespace ws
} // namespace disccord
