#pragma once

#include "Product/Product.h"


namespace Allocation::Domain
{
    class IRepository
    {
    public:
        virtual ~IRepository() = default;

        virtual void Add(const Domain::Batch& batch) = 0;
        virtual [[nodiscard]] std::optional<Product> Get(std::string_view reference) const noexcept = 0;
    };
   
}