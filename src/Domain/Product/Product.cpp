#include "Product.h"

#include "Exceptions/OutOfStock.h"


namespace Allocation::Domain
{

    Product::Product(std::string_view SKU, std::vector<Batch> batches, size_t versionNumber):
        _sku(SKU), _batches(std::move(batches)), _versionNumber(versionNumber)
        {}

    void Product::AddBatch(const Batch& batch) noexcept
    {
        _batches.push_back(batch);
    }

    void Product::AddBatches(const std::vector<Batch>& batches) noexcept
    {
        _batches.insert(_batches.end(), batches.begin(), batches.end());
    }

    std::string Product::Allocate(const OrderLine& line)
    {
        std::vector<std::reference_wrapper<Batch>> sortedBatches;
        sortedBatches.reserve(_batches.size());

        for (auto& batch : _batches)
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

    const std::vector<Batch>& Product::GetBatches() const noexcept
    {
        return _batches;
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