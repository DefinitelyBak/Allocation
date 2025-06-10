#pragma once

#include "Precompile.h"
#include "Product/OrderLine.h"


namespace Allocation::Adapters::Database::Mapper
{

    class OrderLineMapper
    {
    public:
        OrderLineMapper(Poco::Data::Session& session);
        std::vector<Domain::OrderLine> FindByBatchId(size_t batchId);
        void Insert(size_t batchId, const std::vector<Domain::OrderLine>& orders);
        void RemoveByBatchId(size_t batchId);
        void RemoveByBatchesId(std::vector<int> batchesId);
    private:
        Poco::Data::Session& _session;
    };
}