#ifndef SESSION_H
#define SESSION_H

#include <memory>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "leaderboard.h"

namespace beast = boost::beast;
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace http = beast::http;
using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;

class Session: public std::enable_shared_from_this<Session>{
private:
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    Request req_;
    Leaderboard& lb;
    void do_read();
    void on_read(beast::error_code, std::size_t);
    void do_close();

public:
    explicit Session(tcp::socket&& socket, Leaderboard& lb) : stream_(std::move(socket)), lb(lb) {}
    void run() {do_read();}
    friend Response handle_request(const Request& req, Leaderboard& lb);
};

#endif //SESSION_H