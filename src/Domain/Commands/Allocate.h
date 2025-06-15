#pragma once

#include "ICommand.h"


namespace Allocation::Domain::Events
{

    struct Allocate final : public ICommand
    {
        Allocate(std::string orderid, std::string sku, size_t qty) :
            orderid(std::move(orderid)), sku(std::move(sku)), qty(qty) 
        {}
        std::string Name() const override { return "Allocate"; };

        std::string orderid;
        std::string sku;
        size_t qty;
    };
}