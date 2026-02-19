#include "listener.h"
#include "session.h"

Listener::Listener(net::io_context& ioc, tcp::endpoint endpoint, Leaderboard& lb)
    : acceptor_(ioc), socket_(ioc), lb_(lb) {
    acceptor_.open(endpoint.protocol());
    acceptor_.bind(endpoint);
    acceptor_.listen();
    }

void Listener::do_accept() {
    acceptor_.async_accept(
        socket_,
        [self = shared_from_this()](boost::system::error_code ec) {
            if (!ec) {
                std::make_shared<Session>(std::move(self->socket_), self->lb_)->run();
            }
            self->do_accept(); // Accept the next connection
        });
}