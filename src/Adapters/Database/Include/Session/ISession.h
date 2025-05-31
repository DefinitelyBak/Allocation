#pragma once

#include "Precompile.h"


namespace Allocation::Adapters::Database
{
    class ISession
    {
    public:
        virtual ~ISession() = default;

        virtual void begin() = 0;
        virtual void commit() = 0;
        virtual void rollback() = 0;
    };
}