#include "FakeUnitOfWork.h"


namespace Allocation::Services::UoW
{
    FakeUnitOfWork::FakeUnitOfWork() : _repo(Adapters::Repository::FakeRepository())
    {}

    FakeUnitOfWork::FakeUnitOfWork(Adapters::Repository::FakeRepository& repo) : _repo(repo)
    {}

    Domain::IRepository& FakeUnitOfWork::GetProductRepository()
    {
        return *_repo;
    }
}