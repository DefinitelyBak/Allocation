#include "Precompile.h"


namespace Allocation
{

    namespace Domain
    {
        class IRepository;
    }
    typedef std::shared_ptr<Domain::IRepository> IRepositoryPtr;

    namespace Adapters::Database
    {
        class ISession;
    }

    namespace Database
    {
        typedef std::shared_ptr<Adapters::Database::ISession> ISessionPtr;
    }

}