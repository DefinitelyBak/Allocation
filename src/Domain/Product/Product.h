#pragma once

#include "Precompile.h"
#include "Batch.h"


namespace Allocation::Domain
{

    class Product
    {
    public:
        explicit Product(std::string_view SKU,
                        std::vector<Batch> batches,
                        size_t versionNumber);

        [[nodiscard]] std::string Allocate(const OrderLine& line);
        [[nodiscard]] const std::vector<Batch>& GetBatches() const noexcept;
        [[nodiscard]] size_t GetVersionNumber() const noexcept;
        [[nodiscard]] std::string GetSKU() const noexcept;

    private:
        std::string _sku;
        std::vector<Batch> _batches;
        size_t _versionNumber;
    };
}