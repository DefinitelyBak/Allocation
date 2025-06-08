#pragma once

#include "Product/Product.h"


namespace Allocation::Domain
{

    class IRepository
    {
    public:
        virtual ~IRepository() = default;

        virtual void Add(std::shared_ptr<Product> product) = 0;
        virtual [[nodiscard]] std::shared_ptr<Product> Get(std::string_view SKU) const = 0;
    };
}