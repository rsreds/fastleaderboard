#ifndef LISTENER_H
#define LISTENER_H

#include <memory>
#include <boost/asio.hpp>

#include "redis_service.h"

namespace net = boost::asio;
using tcp = net::ip::tcp;

class Listener : public std::enable_shared_from_this<Listener> {
    net::io_context& ioc_;
    tcp::acceptor    acceptor_;
    RedisService&    redis_service_;
    void do_accept();
public:
    Listener(net::io_context& ioc, tcp::endpoint endpoint, RedisService& redis_service);
    void run() { do_accept(); }
};

bool verify_signature(const std::string& player_name, int score, long timestamp, const std::string& signature);

#endif //LISTENER_H