#pragma once

#include "Precompile.h"
#include "Events/IEvent.h"
#include "Commands/ICommand.h"
#include "Domain/Ports/IUnitOfWork.h"


namespace Allocation::Services
{

    class MessageBus
    {
        using UoWPtr = std::shared_ptr<Domain::IUnitOfWork>;
        using EventPtr = Domain::Events::IEventPtr;
        using CommandPtr = Domain::Commands::ICommandPtr;

        using EventHandler = std::function<void(UoWPtr, EventPtr)>;
        using CommandHandler = std::function<std::optional<std::string>(UoWPtr, CommandPtr)>;
        using UoWFactory = std::function<UoWPtr()>;

    public:
        static MessageBus& Instance();

        template<typename T>
        requires std::derived_from<T, Domain::Events::IEvent>
        void SubscribeToEvent(auto&& handler)
        {
            auto& handlers = _eventHandlers[typeid(T)];
            handlers.emplace_back(
                [h = std::forward<decltype(handler)>(handler)]
                (UoWPtr uow, EventPtr event)
                {
                    h(uow, std::static_pointer_cast<T>(event));
                }
            );
        }

        template<typename T>
        requires std::derived_from<T, Domain::Commands::ICommand>
        void SetCommandHandler(auto&& handler)
        {
            _commandHandlers[typeid(T)] =
                [h = std::forward<decltype(handler)>(handler)]
                (UoWPtr uow, CommandPtr cmd)
                {
                    return h(uow, std::static_pointer_cast<T>(cmd));
                };
        }

        [[nodiscard]] std::vector<std::string> Handle(UoWFactory uowFactory, const CommandPtr& command);

    private:
        MessageBus() = default;

        void HandleEvent(UoWPtr uow, EventPtr event, std::queue<CommandPtr>& queue);
        std::optional<std::string> HandleCommand(UoWPtr uow, CommandPtr command, std::queue<CommandPtr>& queue);

        std::unordered_map<std::type_index, std::vector<EventHandler>> _eventHandlers;
        std::unordered_map<std::type_index, CommandHandler> _commandHandlers;

        MessageBus(const MessageBus&) = delete;
        MessageBus& operator=(const MessageBus&) = delete;
    };
}