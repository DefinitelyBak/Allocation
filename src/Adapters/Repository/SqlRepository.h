#pragma once

#include "Precompile.h"
#include "Domain/Ports/IRepository.h"


namespace Allocation::Adapters::Repository
{

    class SqlRepository : public Domain::IRepository
    {
    public:
        SqlRepository(Poco::Data::Session& session);
        void Add(std::shared_ptr<Domain::Product> product) override;
        [[nodiscard]] std::shared_ptr<Domain::Product> Get(std::string_view SKU) override;

        void UpdateVersion(std::string_view SKU, size_t old, size_t newVersion);

    private:
        Poco::Data::Session& _session;
    };
}