#ifndef LISTENER_H
#define LISTENER_H

#include <memory>
#include <boost/asio.hpp>

#include "leaderboard.h"

namespace net = boost::asio;
using tcp = net::ip::tcp;

class Listener: public std::enable_shared_from_this<Listener>{
private:
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    Leaderboard& lb_;
    void do_accept();

public:
    Listener(net::io_context& ioc, tcp::endpoint endpoint, Leaderboard& lb);
    void run(){do_accept();};
};

#endif //LISTENER_H