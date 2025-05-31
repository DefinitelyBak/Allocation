#pragma once

#include "Precompile.h"
#include "ISession.h"


namespace Allocation::Adapters::Database
{
    class PocoSessionAdapter : public ISession
    {
    public:
        PocoSessionAdapter(Poco::Data::Session& session) : _session(session) {}

        void begin() override { _session.begin(); }
        void commit() override { _session.commit(); }
        void rollback() override { _session.rollback(); }

    private:
        Poco::Data::Session& _session;
    };
}