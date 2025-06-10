#include "Product.h"

#include "Exceptions/OutOfStock.h"


namespace Allocation::Domain
{

    Product::Product(std::string_view SKU, std::vector<Batch> batches, size_t versionNumber):
        _sku(SKU),  _versionNumber(versionNumber)
    {
        for (auto& batch : batches)
            _referenceByBatches.insert({std::string(batch.GetReference()), std::move(batch)}); 
    }

    void Product::AddBatch(const Batch& batch) noexcept
    {
        _referenceByBatches.insert_or_assign(std::string(batch.GetReference()), batch);
    }

    void Product::AddBatches(const std::vector<Batch>& batches) noexcept
    {
        for (auto& batch : batches)
            _referenceByBatches.insert_or_assign(std::string(batch.GetReference()), batch);
    }

    std::string Product::Allocate(const OrderLine& line)
    {
        std::vector<std::reference_wrapper<Batch>> sortedBatches;
        sortedBatches.reserve(_referenceByBatches.size());

        for (auto& [_, batch] : _referenceByBatches)
            sortedBatches.emplace_back(batch);

        std::ranges::sort(sortedBatches, BatchETAComparator{});

        for (Batch& batch : sortedBatches | std::views::transform([](auto& b) -> Batch& { return b.get(); }))
        {
            if (batch.CanAllocate(line))
            {
                batch.Allocate(line);
                _versionNumber++;
                return std::string(batch.GetReference());
            }
        }

        throw Exceptions::OutOfStock(line.SKU);
    }

    std::vector<Batch> Product::GetBatches() const noexcept
    {
        std::vector<Batch> result;
        for (auto& [_, batch] : _referenceByBatches)
            result.push_back(batch);

        return result;
    }
    
    size_t Product::GetVersion() const noexcept
    {
        return _versionNumber;
    }

    std::string Product::GetSKU() const noexcept
    {
        return _sku;
    }
}