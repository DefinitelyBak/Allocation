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
        Subscribe<Allocation::Domain::Events::BatchQuantityChanged>(Handlers::ChangeBatchQuantity);
    }

    MessageBus& MessageBus::Instance()
    {
        static MessageBus eventbus;
        return eventbus;
    }

    std::vector<std::string> MessageBus::Handle(UoWFactory uowFactory, const CommandPtr& command)
    {
        std::vector<std::string> result;
        std::queue<Domain::Events::IEventPtr> events;
        events.push(event);

        while (!events.empty())
        {
            auto currentEvent = events.front();
            events.pop();

            for (auto& handler : _subscribers[std::type_index(typeid(*currentEvent))])
            {
                auto uow = uowFactory();
                auto res = handler(uow, currentEvent);
                if (res.has_value())
                    result.push_back(res.value());

                for (auto& newEvent : uow->GetNewEvents())
                    events.push(newEvent);
            }
        }

        return result;
    }

    void MessageBus::HandleEvent(UoWPtr uow, EventPtr event, std::queue<CommandPtr>& queue)
    {

    }

    std::optional<std::string> MessageBus::HandleCommand(UoWPtr uow, CommandPtr command, std::queue<CommandPtr>& queue)
    {
        
    }
}