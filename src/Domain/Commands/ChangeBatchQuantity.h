#pragma once

#include "ICommand.h"


namespace Allocation::Domain::Events
{

    struct ChangeBatchQuantity final : public ICommand
    {
        ChangeBatchQuantity(std::string ref, size_t qty) : ref(std::move(ref)), qty(qty) 
        {}
        std::string Name() const override { return "ChangeBatchQuantity"; };
        
        std::string ref;
        size_t qty;
    };
}