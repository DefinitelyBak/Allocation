#include <gtest/gtest.h>

#include "Precompile.h"
#include "CommonFunctions.h"
#include "Utilities/Common.h"
#include "ApiClient.h"


namespace Allocation::Tests
{
    class RedisSubscription
    {
    public:
        RedisSubscription(Poco::Redis::Client& client, const std::string& channel)
            : _client(client), _channel(channel)
        {
            Poco::Redis::Command subscribe("SUBSCRIBE");
            subscribe.add(channel);
            _client.execute<void>(subscribe);
        }

        // Ждём сообщение в отдельном потоке, вызываем этот метод в цикле с таймаутом
        bool getMessage(std::string& outMessage, int timeoutMs)
        {
            Poco::Redis::ReplyPtr reply;
            auto start = std::chrono::steady_clock::now();

            while (true)
            {
                if (_client.receive(reply))
                {
                    // reply это массив [message, channel, payload]
                    auto arr = Poco::Redis::BulkReply::Ptr(reply);
                    if (arr && arr->size() >= 3)
                    {
                        std::string type = arr->get(0)->toString();
                        std::string channel = arr->get(1)->toString();
                        std::string payload = arr->get(2)->toString();

                        if (type == "message" && channel == _channel)
                        {
                            outMessage = payload;
                            return true;
                        }
                    }
                }

                auto now = std::chrono::steady_clock::now();
                if (now - start > std::chrono::milliseconds(timeoutMs))
                    return false;

                std::this_thread::sleep_for(10ms);
            }
        }

    private:
        Poco::Redis::Client& _client;
        std::string _channel;
    };

    TEST(ExternalEvents, ChangeBatchQuantityLeadsToReallocation)
    {
        Poco::Redis::Client redis("localhost", 6379);

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

        RedisSubscription subscription(redis, "line_allocated");

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

        // Ожидаем сообщение о переаллокации в течение 3 секунд
        std::string message;
        bool gotMessage = false;
        for (int i = 0; i < 30; ++i)  // 30 * 100ms = 3s
        {
            if (subscription.getMessage(message, 100))
            {
                gotMessage = true;
                break;
            }
        }

        ASSERT_TRUE(gotMessage) << "Did not receive reallocation message";

        // Парсим JSON сообщение
        Poco::JSON::Parser parser;
        auto result = parser.parse(message);
        auto obj = result.extract<Poco::JSON::Object::Ptr>();

        ASSERT_EQ(obj->getValue<std::string>("orderid"), orderid);
        ASSERT_EQ(obj->getValue<std::string>("batchref"), laterBatch);
    }
}