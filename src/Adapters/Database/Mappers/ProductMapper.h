#pragma once

#include "Precompile.h"
#include "Domain/Product/Product.h"


namespace Allocation::Adapters::Database::Mapper
{

    class ProductMapper
    {
    public:
        ProductMapper(Poco::Data::Session& session);

        bool IsExists(std::string SKU);
        std::shared_ptr<Domain::Product> FindBySKU(std::string SKU);
        void Update(std::shared_ptr<Domain::Product> product);
        void Insert(std::shared_ptr<Domain::Product> product);
        bool UpdateVersion(std::string SKU, size_t oldVersion, size_t newVersion);

    private:
        Poco::Data::Session& _session;
    };
}