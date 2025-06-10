#include "ProductMapper.h"
#include "BatchMapper.h"


namespace Allocation::Adapters::Database::Mapper
{
    ProductMapper::ProductMapper(Poco::Data::Session& session): _session(session)
    {}

    bool ProductMapper::IsExists(std::string SKU)
    {
        Poco::Nullable<int> dummy;
        int rowsAffected = 0;
        _session << "SELECT 1 FROM products WHERE sku = ?",
            Poco::Data::Keywords::into(dummy),
            Poco::Data::Keywords::use(SKU),
            Poco::Data::Keywords::now;
        return !dummy.isNull();
    }

    std::shared_ptr<Domain::Product> ProductMapper::FindBySKU(std::string SKU)
    {
        BatchMapper _batchMapper(_session);
        std::shared_ptr<Domain::Product> result;
        
        Poco::Nullable<int> version;
        _session << R"(SELECT version_number FROM products WHERE sku = ?)",
                    Poco::Data::Keywords::into(version),
                    Poco::Data::Keywords::use(SKU),
                    Poco::Data::Keywords::now;

        if (version.isNull())
            return result;

        auto batches = _batchMapper.GetBySKU(SKU);
        result = std::make_shared<Domain::Product>(SKU, batches, version.value());
        return result;
    }

    void ProductMapper::Update(std::shared_ptr<Domain::Product> product)
    {
        BatchMapper _batchMapper(_session);
        auto batches = product->GetBatches();

        _batchMapper.Update(batches, product->GetSKU());
    }

    void ProductMapper::Insert(std::shared_ptr<Domain::Product> product)
    {
        BatchMapper _batchMapper(_session);

        std::string sku = product->GetSKU();
        int version = product->GetVersion();

        _session << R"(INSERT INTO products (sku, version_number) VALUES (?, ?))",
            Poco::Data::Keywords::use(sku),
            Poco::Data::Keywords::use(version),
            Poco::Data::Keywords::now;

        _batchMapper.Insert(product->GetBatches());
    }

    bool ProductMapper::UpdateVersion(std::string SKU, size_t oldVersion, size_t newVersion)
    {
        int rowsAffected = 0;
        _session << R"(
            UPDATE products
            SET version_number = ?
            WHERE sku = ? AND version_number = ?
        )",
        Poco::Data::Keywords::use(newVersion),
        Poco::Data::Keywords::use(SKU),
        Poco::Data::Keywords::use(oldVersion),
        Poco::Data::Keywords::limit(rowsAffected),
        Poco::Data::Keywords::now;

        return rowsAffected == 1;
    }
}