#pragma once

#include "Precompile.h"
#include "AbstractUnitOfWork.h"
#include "Adapters/Repository/FakeRepository.h"


namespace Allocation::Services::UoW
{
    class FakeUnitOfWork final : public AbstractUnitOfWork
    {
    public:
        FakeUnitOfWork();
        FakeUnitOfWork(Adapters::Repository::FakeRepository& repo);

        Domain::IRepository& GetProductRepository() override;

    private:
        std::optional<Adapters::Repository::FakeRepository> _repo;
    };
}