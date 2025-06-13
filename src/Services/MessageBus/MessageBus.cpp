#include "MessageBus.h"
#include "Domain/Events/OutOfStock.h"
#include "Handlers.h"


namespace Allocation::Services
{
    MessageBus::MessageBus()
    {
        Subscribe<Allocation::Domain::Events::OutOfStock>(Handlers::SendOutOfStockNotification);
    }

    MessageBus& MessageBus::Instance()
    {
        static MessageBus eventbus;
        return eventbus;
    }

    void MessageBus::Publish(const Domain::Events::IEventPtr& event)
    {
        auto it = _subscribers.find(std::type_index(typeid(*event)));
        if (it != _subscribers.end())
            for (auto& handler : it->second)
                handler(event);      
    }
}