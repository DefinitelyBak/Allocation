#pragma once

#include "Precompile.h"
#include "Domain/Product/Product.h"


namespace Allocation::Adapters::Database::Mapper
{

    class ProductMapper
    {
    public:
        ProductMapper()
        {}

        std::shared_ptr<Domain::Product> FindBySKU()
        {}
    };
}