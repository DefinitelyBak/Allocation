#include "Services.h"
#include "Exceptions/InvalidSku.h"
#include "Domain/Ports/IRepository.h"


namespace Allocation::Services
{
    void AddBatch(Domain::IUnitOfWork& uow,
        std::string_view ref, std::string_view SKU, size_t qty,
        std::optional<std::chrono::year_month_day> ETA)
    {
        auto& repo = uow.GetProductRepository();
        std::shared_ptr<Domain::Product> product = repo.Get(SKU);
        if (!product)
        {
            product = std::make_shared<Domain::Product>(SKU);
            repo.Add(product);
        }

        product->AddBatch(Domain::Batch(ref, SKU, qty, ETA));
        uow.Commit();
    }

    std::string Allocate(
        Domain::IUnitOfWork& uow,
        std::string orderid, std::string sku, int qty)
    {
        Domain::OrderLine line(orderid, sku, qty);

        auto& products = uow.GetProductRepository();
        auto product = products.Get(sku);
        if (!product)
            throw InvalidSku(std::format("Invalid sku {}", sku));
        
        auto ref = product->Allocate(line);
        products.Add(product);
        uow.Commit();
        
        return ref;
    }
}