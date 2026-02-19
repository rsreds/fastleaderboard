#ifndef LISTENER_H
#define LISTENER_H

#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp as tcp;
using boost::

class Listener: public std::enable_shared_from_this<Listener>{
private:
    tcp::acceptor acceptor_;
    tcp::socket socket_;

public:
    Listener(net)
};

#endif //LISTENER_H