#include "Services.h"
#include "ModelLayer.h"
#include "InvalidSku.h"
#include "Domain/Include/Ports/IRepository.h"


namespace Allocation::Services
{
    bool IsValidSku(std::string sku, const std::vector<Domain::Batch>& batches)
    {
        auto it = std::find_if(batches.begin(), batches.end(),
        [&sku](const auto& batch){return batch.GetSKU() == sku;});

        return it != batches.end();
    }

    std::string Allocate(
        const Domain::OrderLine& line,
        IRepositoryPtr repo,
        Poco::Data::Session& session)
    {
        session.begin();
        auto batches = repo->List();
        if (!IsValidSku(line.GetSKU(), batches))
            throw InvalidSku(std::format("Invalid article {}", line.GetSKU()));

        auto batchref = Domain::Allocate(line, batches.begin(), batches.end());
        session.commit();
        return batchref;
    }
}