#pragma once

#include "Precompile.h"


namespace Allocation::Adapters::Database
{

    void InitDatabase(Poco::Data::Session& session, const std::string& dbType);
    void DropDatabase(Poco::Data::Session& session);
}