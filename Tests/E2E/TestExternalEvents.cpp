#include <gtest/gtest.h>

#include "Precompile.h"
#include "CommonFunctions.h"
#include "Utilities/Common.h"
#include "RedisClient.h"
#include "ApiClient.h"


namespace Allocation::Tests
{
    TEST(ExternalEvents, ChangeBatchQuantityLeadsToReallocation)
    {
        Poco::Path exePath(Poco::Path::current());
        exePath.append("Allocation.ini");

        if (!Poco::File(exePath).exists())
        {
            FAIL() << "INI file 'Allocation.ini' not found in " << exePath.toString();
        }

        Poco::AutoPtr<Poco::Util::IniFileConfiguration> pConf(
            new Poco::Util::IniFileConfiguration(exePath.toString()));
        std::string host = pConf->getString("redis.host", "127.0.0.1");
        int port = pConf->getInt("redis.port", 6379);

        std::string orderid = RandomOrderId();
        std::string sku = RandomSku();
        std::string earlierBatch = RandomBatchRef("old");
        std::string laterBatch = RandomBatchRef("newer");

        using namespace std::chrono;
        const year_month_day today(2011y, December, 1d);
        const auto tomorrow = today + days(1);

        ApiClient::PostToAddBatch(earlierBatch, sku, 10, today);
        ApiClient::PostToAddBatch(laterBatch, sku, 10, tomorrow);
        auto response = ApiClient::PostToAllocate(orderid, sku, 10);
        EXPECT_EQ(response, earlierBatch);

        RedisClient subscription(host, port, "line_allocated");

        Poco::Redis::Client redis(host, port);
        {
            Poco::JSON::Object obj;
            obj.set("batchref", earlierBatch);
            obj.set("qty", 5);

            std::stringstream ss;
            obj.stringify(ss);

            Poco::Redis::Command publish("PUBLISH");
            publish.add("change_batch_quantity");
            publish.add(ss.str());

            redis.execute<void>(publish);
        }

        std::string message;
        bool gotMessage = false;
        for (int i = 0; i < 30; ++i)
        {
            if (subscription.readNextMessage(message, 100))
            {
                gotMessage = true;
                break;
            }
        }

        ASSERT_TRUE(gotMessage) << "Did not receive reallocation message";

        Poco::JSON::Parser parser;
        auto result = parser.parse(message);
        auto obj = result.extract<Poco::JSON::Object::Ptr>();

        ASSERT_EQ(obj->getValue<std::string>("orderid"), orderid);
        ASSERT_EQ(obj->getValue<std::string>("batchref"), laterBatch);
    }
}