#include "HandlerFactory.h"
#include "Handlers/AllocateHandler.h"


namespace Allocation::Infrastructure::Server
{

    Poco::Net::HTTPRequestHandler* HandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
    {
        if (request.getURI() == "/allocate" && request.getMethod() == "POST")
        {
                return new AllocateHandler;
        }
            return nullptr;
    }

}        
