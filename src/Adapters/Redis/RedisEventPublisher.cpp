#include "RedisEventPublisher.h"


namespace Allocation::Adapters::Redis
{
    RedisEventPublisher::RedisEventPublisher(const std::string& host, int port) : _client(host, port)
    {}

    void RedisEventPublisher::Publish(const std::string& channel, std::shared_ptr<Domain::Events::Allocated> event)
    {
        //Poco::Util::Application::instance().logger().debug("publishing: channel=%s, event=%s", channel, event->Name());

        Poco::JSON::Object json;
        json.set("sku", event->SKU);
        json.set("qty", event->qty);
        json.set("batchref", event->batchref);

        std::stringstream ss;
        json.stringify(ss);

        Poco::Redis::Command publish("PUBLISH");
        publish.add(channel);
        publish.add(ss.str());

        _client.sendCommand(publish);
    }
}