#include "Handlers.h"
#include "Adapters/Email/Email.h"
#include "Domain/Product/Product.h"
#include "Exceptions/InvalidSku.h"


namespace Allocation::Services::Handlers
{
    std::optional<std::string> SendOutOfStockNotification(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::OutOfStock> event)
    {
        Adapters::Email::SendMail("stock@made.com", std::format("Out of stock for {}", event->SKU));

        return std::nullopt;
    }

    std::optional<std::string> AddBatch(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::BatchCreated> event)
    {
        auto& repo = uow.GetProductRepository();
        std::shared_ptr<Domain::Product> product = repo.Get(event->sku);
        if (!product)
            product = std::make_shared<Domain::Product>(event->sku);

        product->AddBatch(Allocation::Domain::Batch(event->ref, event->sku, event->qty, event->eta));
        repo.Add(product);
        uow.Commit();

        return std::nullopt;
    }

    std::optional<std::string> Allocate(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::AllocationRequired> event)
    {
        Domain::OrderLine line(std::string(event->orderid), std::string(event->sku), event->qty);

        auto& products = uow.GetProductRepository();
        auto product = products.Get(event->sku);
        if (!product)
            throw Exceptions::InvalidSku(event->sku);
        
        auto ref = product->Allocate(line);
        products.Add(product);
        uow.Commit();
        
        return ref;
    }

    std::optional<std::string> ChangeBatchQuantity(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::BatchQuantityChanged> event)
    {
        return std::nullopt;
    }
}