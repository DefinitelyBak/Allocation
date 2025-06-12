#include "MessageBus.h"


namespace Allocation::Services
{
    EventBus& EventBus::Instance()
    {
        static EventBus eventbus;
        return eventbus;
    }

    void EventBus::Publish(const Domain::Events::IEventPtr& event)
    {
        auto it = _subscribers.find(std::type_index(typeid(*event)));
        if (it != _subscribers.end())
            for (auto& handler : it->second)
                handler(event);      
    }
}