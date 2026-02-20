#ifndef REDIS_SERVICE_H
#define REDIS_SERVICE_H

#include <boost/redis.hpp>
#include <boost/asio.hpp>
#include "score.h"

namespace redis = boost::redis;
namespace net = boost::asio;

class RedisService
{
private:
    std::shared_ptr<redis::connection> _redis_connection;
    
    static std::string get_redis_host() {
        const char* env_host = std::getenv("REDIS_HOST");
        return env_host ? std::string(env_host) : "localhost";
    }
    
    static std::string get_redis_port() {
        const char* env_port = std::getenv("REDIS_PORT");
        return env_port ? std::string(env_port) : "6379";
    }
    
public:
    explicit RedisService(net::io_context&);
    net::awaitable<void> submit_score(const std::string& leaderboard_id, const std::string& player_id, int score, time_t timestamp);
    net::awaitable<std::vector<Score>> get_top_scores(const std::string& leaderboard_id, std::size_t n);

};



#endif // REDIS_SERVICE_H