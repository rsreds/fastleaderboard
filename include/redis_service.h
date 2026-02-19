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
    static constexpr std::string_view REDIS_HOST = "93.44.134.143";
    static constexpr std::string_view REDIS_PORT = "6379";
public:
    explicit RedisService(net::io_context&);
    net::awaitable<void> submit_score(const std::string& leaderboard_id, const std::string& player_id, int score, time_t timestamp);
    net::awaitable<std::vector<Score>> get_top_scores(const std::string& leaderboard_id, std::size_t n);

};



#endif // REDIS_SERVICE_H