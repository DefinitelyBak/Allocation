#pragma once

#include "Precompile.h"
#include "Product/Batch.h"


namespace Allocation::Adapters::Database::Mapper
{

    class BatchMapper
    {
    public:
        explicit BatchMapper(Poco::Data::Session& session);

        std::vector<Domain::Batch> GetBySKU(const std::string& SKU);
        void Update(const std::vector<Domain::Batch>& batches, const std::string& SKU);
        void Insert(const std::vector<Domain::Batch>& batches);

    private:
        void DeleteBatches(std::string SKU);
        int GetIdBatch(std::string reference);
        std::vector<int> GetIdBatches(std::string SKU);

        Poco::Data::Session& _session;
    };
}