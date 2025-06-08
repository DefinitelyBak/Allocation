#include "TrackingRepository.h"


namespace Allocation::Adapters::Repository
{

    TrackingRepository::TrackingRepository(Domain::IRepository& repo) : _repo(repo)
    {};

    void TrackingRepository::Add(std::shared_ptr<Domain::Product> product)
    {
        _repo.Add(product);
        if (!_seen.contains(product->GetReference()))
            _seenObjByOldVersion[product->GetReference()] = product->GetVersion();
        _seen[product->GetReference()] = product;
    }
    
    std::shared_ptr<Domain::Product> TrackingRepository::Get(std::string_view SKU) const
    {
        auto product = _repo.Get(SKU);
        if (product)
        {
            _seen[product->GetReference()] = product;
            _seenObjByOldVersion[product->GetReference()] = product->GetVersion(); 
        }

        return product;
    }

    std::vector<std::shared_ptr<Domain::Product>> TrackingRepository::GetSeen() const noexcept
    {
        std::vector<std::shared_ptr<Product>> result;
        result.reserve(_seen.size());
        for (const auto& [_, product] : _seen)
            result.push_back(product);

        return result;
    }

    std::vector<std::tuple<std::string, size_t, size_t>> TrackingRepository::GetChangedVersions() const noexcept
    {
        std::vector<std::tuple<std::string, size_t, size_t>> result;

        for(auto& [sku, prod] : _seen)
        {
            if(prod->GetVersion() == _seenObjByOldVersion[sku])
                continue;
            result.emplace_back(sku, _seenObjByOldVersion[sku], prod->GetVersion());
        }

        return result;
    }
}