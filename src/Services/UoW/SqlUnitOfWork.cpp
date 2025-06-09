#include "SqlUnitOfWork.h"
#include "Adapters/Repository/SqlRepository.h"
#include "Adapters/Repository/TrackingRepository.h"
#include "Session/SessionPool.h"


namespace Allocation::Services::UoW
{
    struct SqlUnitOfWork::Impl
    {
        Poco::Data::Session session;
        Poco::Data::Transaction transaction;
        Adapters::Repository::TrackingRepository tracking;
        Adapters::Repository::SqlRepository repository;

        Impl()
            : session(Adapters::Database::SessionPool::Instance().GetSession()),
              transaction(session, true),
              repository(session),
              tracking(repository)
        {}

        void commit()
        {
            for (auto& [sku, old, newVersion] : tracking.GetChangedVersions())
                repository.UpdateVersion(sku, old, newVersion);
            
            transaction.commit();
        }
        void rollback() { transaction.rollback(); }
        Domain::IRepository& getRepo() { return tracking; }
    };

    SqlUnitOfWork::SqlUnitOfWork() : _impl(std::make_unique<Impl>()) {}
    SqlUnitOfWork::~SqlUnitOfWork() = default;

    void SqlUnitOfWork::Commit()
    {
        _impl->commit();
        AbstractUnitOfWork::Commit();
    }

    Poco::Data::Session& SqlUnitOfWork::GetSession() const noexcept
    {
        return _impl->session;
    }

    void SqlUnitOfWork::RollBack()
    {
        _impl->rollback();
        AbstractUnitOfWork::RollBack();
    }

    Domain::IRepository& SqlUnitOfWork::GetProductRepository()
    {
        return _impl->getRepo();
    }
}