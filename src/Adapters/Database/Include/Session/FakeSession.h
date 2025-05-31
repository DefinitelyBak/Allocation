#pragma once

#include "Precompile.h"
#include "ISession.h"


namespace Allocation::Adapters::Database
{

    class FakeSession : public ISession
    {
    public:
        void begin() override {}
        void commit() override {}
        void rollback() override {}
    };
}