#include "player.h"
#include "score.h"
#include "leaderboard.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/json.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    const auto address = boost::asio::ip::make_address("0.0.0.0");
    const unsigned short port = argc > 1 ? static_cast<unsigned short>(std::stoi(argv[1])) : 8080;
    const int threads = argc > 2 ? std::stoi(argv[2]) : 2;

    Leaderboard lb;
    boost::asio::::io_context ioc{threads};

    std::make_shared<Listener>(ioc, tcp::endpoint{address, port}, lb)->run();

    std::cout << "Leaderboard server listening on 0.0.0.0:" << port
              << " (" << threads << " threads)\n";

    std::vector<std::thread> pool;
    pool.reserve(threads - 1);
    for (int i = 0; i < threads - 1; ++i)
        pool.emplace_back([&ioc] { ioc.run(); });
    ioc.run();
    for (auto& t : pool) t.join();
}