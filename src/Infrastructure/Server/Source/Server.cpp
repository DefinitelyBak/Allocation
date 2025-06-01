#include "Server.h"

#include "HandlerFactory.h"
#include "Adapters/Database/Include/Session/SessionPool.h"


namespace Allocation::Infrastructure::Server
{
    int ServerApp::main(const std::vector<std::string>&)
    {
        Adapters::Database::SessionPool::Instance().Configure("SQLite", ":memory:");
        Poco::UInt16 port = 8080;
        Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
        pParams->setMaxQueued(100);
        pParams->setMaxThreads(16);
        Poco::Net::ServerSocket svs(port);
        Poco::Net::HTTPServer server(new HandlerFactory, svs, pParams);

        server.start();
        waitForTerminationRequest();
        server.stop();

        return Application::EXIT_OK;
    };
}