#pragma once

#include "Precompile.h"
#include "Batch.h"


namespace Allocation::Domain
{

    class Product
    {
    public:
        explicit Product(std::string_view SKU,
                        std::vector<Batch> batches = {},
                        size_t versionNumber = 0);

        [[nodiscard]] std::string Allocate(const OrderLine& line);
        void AddBatch(const Batch& batch) noexcept;
        [[nodiscard]] const std::vector<Batch>& GetBatches() const noexcept;
        [[nodiscard]] size_t GetVersion() const noexcept;
        [[nodiscard]] std::string GetSKU() const noexcept;

    private:
        std::string _sku;
        std::vector<Batch> _batches;
        size_t _versionNumber;
    };
}