#pragma once

#include "Precompile.h"


namespace Allocation::Tests
{
    class RedisClient {
    public:
        RedisClient(const std::string& host, int port, std::string chanel) 
            : _client(host, port) 
        {
            Poco::Redis::Command subscribe("SUBSCRIBE");
            subscribe << chanel;
            _client.execute<void>(subscribe);
        }

        bool readNextMessage(std::string& outMessage, int timeoutMs = 1000)
        {
            Poco::Redis::Array reply;
            
            _client.setReceiveTimeout(timeoutMs);
            _client.readReply(reply);
            if (!reply.isNull())
            {
                if (reply.size() >= 3)
                {
                    std::string type = reply.get<Poco::Redis::BulkString>(0).value();
                    std::string channel = reply.get<Poco::Redis::BulkString>(1).value();
                    outMessage = reply.get<Poco::Redis::BulkString>(2).value();
                    
                    if (type == "message")
                        return true;
                }
            }
            return false;
        }

    private:
        Poco::Redis::Client _client;
    };
}