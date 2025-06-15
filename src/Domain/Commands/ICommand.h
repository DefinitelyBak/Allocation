#pragma once

#include "Precompile.h"


namespace Allocation::Domain::Commands
{
    
    struct ICommand
    {
        virtual ~ICommand() = default;
        virtual std::string Name() const = 0;
    };

    using ICommandPtr = std::shared_ptr<ICommand>;
}