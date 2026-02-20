#include "player.h"
#include "score.h"
#include "leaderboard.h"
#include "listener.h"
#include "redis_service.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/json.hpp>
#include <iostream>

namespace net = boost::asio;

int main(int argc, char* argv[]) {
    const auto address = net::ip::make_address("0.0.0.0");
    const unsigned short port = argc > 1 ? static_cast<unsigned short>(std::stoi(argv[1])) : 8080;
    const int threads = argc > 2 ? std::stoi(argv[2]) : 2;

    net::io_context ioc{threads};

    Leaderboard lb("TEST");
    RedisService redis_service(ioc);
    std::cerr << "Creating listener..." << std::endl;
    auto listener = std::make_shared<Listener>(ioc, tcp::endpoint{address, port}, lb, redis_service);
    std::cerr << "Starting listener..." << std::endl;
    listener->run();
    std::cerr << "Listener started" << std::endl;

    std::cerr << "Leaderboard server listening on 0.0.0.0:" << port
              << " (" << threads << " threads)" << std::endl;

    std::cerr << "Starting io_context threads..." << std::endl;
    std::vector<std::thread> pool;
    pool.reserve(threads - 1);
    for (int i = 0; i < threads - 1; ++i)
        pool.emplace_back([&ioc] { ioc.run(); });
    std::cerr << "Running main io_context..." << std::endl;
    ioc.run();
    std::cerr << "io_context stopped, joining threads..." << std::endl;
    for (auto& t : pool) t.join();
}