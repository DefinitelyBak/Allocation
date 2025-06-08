#pragma once

#include "SqlRepository.h"
#include "Adapters/Database/Mappers/ProductMapper.h"


namespace Allocation::Adapters::Repository
{

    SqlRepository::SqlRepository(Poco::Data::Session& session): _session(session)
    {}

    void SqlRepository::Add(std::shared_ptr<Domain::Product> product)
    {
        /*
        Database::Mapper::ProductMapper mapper(_session);

        if (auto id = mapper.GetBatchId(batch.GetReference()); id.has_value())
            mapper.Update(id.value(), batch);
        else
            mapper.Insert(batch);
            */
    }

    std::shared_ptr<Domain::Product> SqlRepository::Get(std::string_view SKU) const
    {
        /*
        Database::Mapper::BatchMapper mapper(_session);

        return mapper.FindByReference(reference);
        */
       return nullptr;
    }

    void SqlRepository::UpdateVersion(std::string_view SKU, size_t old, size_t newVersion)
    {

    }

}