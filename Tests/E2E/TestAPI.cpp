#include "Forwards.h"
#include "Precompile.h"
#include "CommonFunctions.h"

#include <gtest/gtest.h>


namespace Allocation::Tests
{
    using namespace Poco;
    using namespace Poco::Net;
    using namespace Poco::JSON;

    // основное API
    std::string api_url()
    {
        return "http://localhost:8080";
    }

    TEST(ApiTests, HappyPathReturns201AndAllocatedBatch)
    {
        std::string sku = RandomSku();
        std::string othersku = RandomSku("other");
        std::string earlybatch = RandomBatchRef("1");
        std::string laterbatch = RandomBatchRef("2");
        std::string otherbatch = RandomBatchRef("3");

        // Подразумевается, что у тебя есть функция add_stock в C++ или подготовка состояния БД

        // Формируем JSON для POST /allocate
        Object::Ptr obj = new Object;
        obj->set("orderid", RandomOrderId());
        obj->set("sku", sku);
        obj->set("qty", 3);

        std::stringstream body;
        obj->stringify(body);

        URI uri(api_url() + "/allocate");
        HTTPClientSession session(uri.getHost(), uri.getPort());
        HTTPRequest request(HTTPRequest::HTTP_POST, uri.getPath(), HTTPMessage::HTTP_1_1);
        request.setContentType("application/json");
        request.setContentLength((int)body.str().length());

        std::ostream& os = session.sendRequest(request);
        os << body.str();

        HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);

        std::stringstream result;
        StreamCopier::copyStream(rs, result);

        EXPECT_EQ(response.getStatus(), HTTPResponse::HTTP_CREATED);

        Parser parser;
        Poco::Dynamic::Var parsed = parser.parse(result);
        Object::Ptr json = parsed.extract<Object::Ptr>();
        std::string batchref = json->getValue<std::string>("batchref");

        EXPECT_EQ(batchref, earlybatch);
    }

    TEST(ApiTests, UnhappyPathReturns400AndErrorMessage)
    {
        std::string unknown_sku = RandomSku();
        std::string orderid = RandomOrderId();

        Object::Ptr obj = new Object;
        obj->set("orderid", orderid);
        obj->set("sku", unknown_sku);
        obj->set("qty", 20);

        std::stringstream body;
        obj->stringify(body);

        URI uri(api_url() + "/allocate");
        HTTPClientSession session(uri.getHost(), uri.getPort());
        HTTPRequest request(HTTPRequest::HTTP_POST, uri.getPath(), HTTPMessage::HTTP_1_1);
        request.setContentType("application/json");
        request.setContentLength((int)body.str().length());

        std::ostream& os = session.sendRequest(request);
        os << body.str();

        HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);

        std::stringstream result;
        StreamCopier::copyStream(rs, result);

        EXPECT_EQ(response.getStatus(), HTTPResponse::HTTP_BAD_REQUEST);

        Parser parser;
        Poco::Dynamic::Var parsed = parser.parse(result);
        Object::Ptr json = parsed.extract<Object::Ptr>();

        std::string message = json->getValue<std::string>("message");
        EXPECT_EQ(message, "Invalid sku " + unknown_sku);
    }

}