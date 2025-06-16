#pragma once

#include <Poco/Redis/Client.h>
#include <Poco/Redis/SubscribeMessage.h>
#include <thread>


class RedisClient
{
public:
    using MessageHandler = std::function<void(const std::string& channel, const std::string& message)>;

    RedisClient(const std::string& host, int port);
    ~RedisClient();

    void Subscribe(const std::string& channel, MessageHandler handler);
    void Publish(const std::string& channel, const Poco::JSON::Object::Ptr& message);

private:
    Poco::Redis::Client _client;
    std::unique_ptr<Poco::Redis::AsyncReader> _reader;
    std::mutex _mutex;
};


void SubscribeAndListen(Poco::Redis::Client& client, const std::string& channel)
{
    client.subscribe(channel);
    while (true)
    {
        Poco::Redis::RedisType::Ptr msg = client.readReply();
        auto* subMsg = dynamic_cast<Poco::Redis::SubscribeMessage*>(msg.get());
        if (subMsg && subMsg->channel() == channel)
        {
            std::string jsonStr = subMsg->message();
            Poco::JSON::Parser parser;
            auto obj = parser.parse(jsonStr).extract<Poco::JSON::Object::Ptr>();
            std::cout << "orderid: " << obj->getValue<std::string>("orderid")
                      << ", batchref: " << obj->getValue<std::string>("batchref") << std::endl;
            break;
        }
    }
}