#pragma once

#include "Domain/Ports/IUnitOfWork.h"
#include "Domain/Events/OutOfStock.h"
#include "Domain/Events/AllocationRequired.h"
#include "Domain/Events/BatchCreated.h"
#include "Domain/Events/BatchQuantityChanged.h"


namespace Allocation::Services::Handlers
{
    std::optional<std::string> SendOutOfStockNotification(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::OutOfStock> event);

    std::optional<std::string> AddBatch(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::BatchCreated> event);

    std::optional<std::string> Allocate(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::AllocationRequired> event);

    std::optional<std::string> ChangeBatchQuantity(Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::BatchQuantityChanged> event);
}