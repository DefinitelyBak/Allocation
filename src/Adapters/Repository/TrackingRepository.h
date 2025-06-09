#pragma once

#include "Precompile.h"
#include "Domain/Ports/IRepository.h"


namespace Allocation::Adapters::Repository
{

    class TrackingRepository : public Domain::IRepository
    {
    public:
        TrackingRepository(Domain::IRepository& repo);

        void Add(std::shared_ptr<Domain::Product> product) override;
        [[nodiscard]] std::shared_ptr<Domain::Product> Get(std::string_view SKU) override;

        [[nodiscard]] std::vector<std::shared_ptr<Domain::Product>> GetSeen() const noexcept;
        std::vector<std::tuple<std::string, size_t, size_t>> GetChangedVersions() const noexcept;

    private:
        Domain::IRepository& _repo;
        std::unordered_map<std::string, std::shared_ptr<Domain::Product>> _seen;
        std::unordered_map<std::string, size_t> _seenObjByOldVersion;
    };
}