#pragma once

#include "Precompile.h"
#include "Events/IEvent.h"
#include "Domain/Ports/IUnitOfWork.h"


namespace Allocation::Services
{

    class MessageBus
    {
    public:
        static MessageBus& Instance();

        template<typename T>
        void Subscribe(std::function<std::optional<std::string>(Domain::IUnitOfWork& uow, std::shared_ptr<T>)> handler)
        {
            auto& handlers = _subscribers[typeid(T)];
            handlers.push_back([handler](Domain::IUnitOfWork& uow, Domain::Events::IEventPtr event) -> std::optional<std::string>
            {
                return handler(uow, std::static_pointer_cast<T>(event));
            });
        }

        std::vector<std::string> Handle(Domain::IUnitOfWork& uow, const Domain::Events::IEventPtr& event);

    private:
        MessageBus();

        std::unordered_map<std::type_index, std::vector<std::function<std::optional<std::string>(Domain::IUnitOfWork&, Domain::Events::IEventPtr)>>> _subscribers;
    };
}