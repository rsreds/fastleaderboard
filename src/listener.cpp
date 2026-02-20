#include "listener.h"
#include "parsed_request.h"
#include "json_helpers.h"
#include <boost/json.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

net::awaitable<Response> handle_request(const Request& req, Leaderboard& lb, RedisService& redis_service) {
    auto ver = req.version();
    auto ka  = req.keep_alive();
    auto pr  = parse_request(req);

    using namespace json_helpers;

    std::cerr << "Received request: method=" << pr.method << " path=" << pr.path << "\n";

    if (pr.method == "OPTIONS") {
        Response res{http::status::no_content, ver};
        res.keep_alive(ka);
        res.set(http::field::access_control_allow_origin, "*");
        res.set(http::field::access_control_allow_methods, "GET, POST, DELETE, OPTIONS");
        res.set(http::field::access_control_allow_headers, "Content-Type");
        res.prepare_payload();
        co_return res;
    }

    if (pr.method == "POST" && pr.path == "/leaderboard/submit") {
        try {
            std::cerr << "Parsing submit request body\n";
            auto json_body      = boost::json::parse(pr.body).as_object();
            std::string player  = json_body.at("player_name").as_string().c_str();
            int score          = static_cast<int>(json_body.at("score").as_int64());
            std::cerr << "Submitting score to Redis: player=" << player << " score=" << score << "\n";
            co_await redis_service.submit_score(lb.id(), player, score, std::time(nullptr));
            std::cerr << "Score submitted successfully\n";
            co_return make_json_response(ver, ka, {{"message", "Score submitted"}});
        } catch (const boost::system::system_error& e) {
            std::cerr << "Redis error: " << e.what() << "\n";
            co_return make_error_response(ver, ka, http::status::internal_server_error,
                std::string(e.what()));
        } catch (const std::exception& e) {
            co_return make_error_response(ver, ka, http::status::bad_request,
                "Invalid request body: " + std::string(e.what()));
        }
    }

    if (pr.method == "GET" && pr.path == "/leaderboard/top") {
        int n = 10;
        if (pr.query_params.contains("n"))
            n = std::stoi(pr.query_params.at("n"));

        auto top = co_await redis_service.get_top_scores(lb.id(), n);

        boost::json::array arr;
        for (size_t i = 0; i < top.size(); ++i) {
            boost::json::object entry;
            entry["rank"]   = i + 1;
            entry["player"] = top[i].player_uuid();
            entry["score"]  = top[i].score();
            arr.push_back(entry);
        }
        co_return make_json_response(ver, ka, {{"top", arr}});
    }

    co_return make_error_response(ver, ka, http::status::not_found, "Unknown endpoint");
}

net::awaitable<void> run_session(tcp::socket socket, Leaderboard& lb, RedisService& redis) {
    std::cerr << "Session started\n";
    beast::tcp_stream stream(std::move(socket));
    beast::flat_buffer buffer;

    for (;;) {
        Request req;
        std::cerr << "Reading request...\n";
        auto [ec, _] = co_await http::async_read(stream, buffer, req,
            net::as_tuple(net::use_awaitable));

        std::cerr << "Read complete, ec=" << ec.message() << "\n";
        if (ec == http::error::end_of_stream) break;
        if (ec) co_return;

        std::cerr << "Handling request...\n";
        auto res  = co_await handle_request(req, lb, redis);
        bool keep = res.keep_alive();

        std::cerr << "Writing response...\n";
        co_await http::async_write(stream, res, net::use_awaitable);
        std::cerr << "Response written\n";

        if (!keep) break;
    }

    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_send, ec);
}

Listener::Listener(net::io_context& ioc, tcp::endpoint endpoint, Leaderboard& lb, RedisService& redis_service)
    : ioc_(ioc), acceptor_(ioc), lb_(lb), redis_service_(redis_service) {
    std::cerr << "Listener constructor: opening endpoint " << endpoint << "\n";
    acceptor_.open(endpoint.protocol());
    std::cerr << "Listener constructor: setting reuse_address\n";
    acceptor_.set_option(net::socket_base::reuse_address(true));
    std::cerr << "Listener constructor: binding\n";
    acceptor_.bind(endpoint);
    std::cerr << "Listener constructor: listening\n";
    acceptor_.listen(net::socket_base::max_listen_connections);
    std::cerr << "Listener constructor: complete\n";
}

void Listener::do_accept() {
    std::cerr << "Waiting for connection..." << std::endl;
    acceptor_.async_accept(
        net::make_strand(ioc_),
        [self = shared_from_this()](beast::error_code ec, tcp::socket socket) {
            try {
                std::cerr << "Connection received! ec=" << ec.message() << std::endl;
                if (!ec) {
                    std::cerr << "Spawning session coroutine" << std::endl;
                    net::co_spawn(self->ioc_,
                        run_session(std::move(socket), self->lb_, self->redis_service_),
                        [](std::exception_ptr e) {
                            if (e) {
                                try { std::rethrow_exception(e); }
                                catch (const std::exception& ex) {
                                    std::cerr << "Session exception: " << ex.what() << std::endl;
                                }
                            }
                        });
                }
                self->do_accept();
            } catch (const std::exception& e) {
                std::cerr << "Accept callback exception: " << e.what() << std::endl;
            }
        });
}