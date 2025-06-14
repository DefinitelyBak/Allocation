#include "MessageBus.h"
#include "Domain/Events/OutOfStock.h"
#include "Handlers.h"


namespace Allocation::Services
{
    MessageBus::MessageBus()
    {
        Subscribe<Allocation::Domain::Events::OutOfStock>(Handlers::SendOutOfStockNotification);
        Subscribe<Allocation::Domain::Events::AllocationRequired>(Handlers::Allocate);
        Subscribe<Allocation::Domain::Events::BatchCreated>(Handlers::AddBatch);
    }

    MessageBus& MessageBus::Instance()
    {
        static MessageBus eventbus;
        return eventbus;
    }

    std::vector<std::string> MessageBus::Handle(Domain::IUnitOfWork& uow, const Domain::Events::IEventPtr& event)
    {
        std::vector<std::string> result;
        std::queue<Domain::Events::IEventPtr> events;
        events.push(event);

        while (!events.empty())
        {
            auto current_event = events.front();
            events.pop();

            for (auto& handler : _subscribers[std::type_index(typeid(*current_event))])
            {
                auto res = handler(uow, current_event);
                if (res.has_value())
                    result.push_back(res.value());

                for (auto& new_event : uow.GetNewEvents())
                    events.push(new_event);
            }
        }

        return result;
    }
}