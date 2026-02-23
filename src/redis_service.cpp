#include "redis_service.h"
#include "leaderboard.h"
#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

RedisService::RedisService(net::io_context& ioc) {
    redis::config config;
    config.addr.host = get_redis_host();
    config.addr.port = get_redis_port();
    std::cerr << "Connecting to Redis at " << config.addr.host << ":" << config.addr.port << "\n";

    _redis_connection = std::make_shared<redis::connection>(ioc);
    _redis_connection->async_run(config, {}, [](boost::system::error_code ec) {
        if (ec) std::cerr << "Redis connection error: " << ec.message() << "\n";
    });
}

net::awaitable<void> RedisService::submit_score(const std::string& leaderboard_id, const std::string& player_id, int score, time_t timestamp) {
    std::cerr << "Building Redis request..." << std::endl;
    Score new_score(player_id, score, timestamp);
    redis::request req;
    req.push("MULTI");
    req.push("ZADD", "leaderboard:" + leaderboard_id, "GT", score, player_id);
    req.push("ZADD", "leaderboard:" + leaderboard_id + ":player:" + player_id,
         score, new_score.to_redis_member());
    req.push("EXEC");

    std::cerr << "Preparing Redis response..." << std::endl;
    redis::response<
        redis::ignore_t,  // MULTI
        redis::ignore_t,  // ZADD
        redis::ignore_t,  // ZADD
        redis::ignore_t   // EXEC
    > resp;

    std::cerr << "Executing Redis request..." << std::endl;
    boost::system::error_code ec;
    co_await _redis_connection->async_exec(req, resp, net::redirect_error(net::use_awaitable, ec));
    if (ec) {
        std::cerr << "Redis exec error: " << ec.message() << std::endl;
        throw boost::system::system_error(ec);
    }
    std::cerr << "Redis request complete" << std::endl;
}

net::awaitable<std::vector<Score>> RedisService::get_top_scores(const std::string& leaderboard_id, std::size_t n) {
    redis::request req;
    req.push("ZREVRANGE", "leaderboard:" + leaderboard_id, 0, (int)n - 1, "WITHSCORES");

    redis::response<std::vector<std::string>> resp;
    co_await _redis_connection->async_exec(req, resp, net::use_awaitable);

    auto& flat = std::get<0>(resp).value();
    std::cerr << "Redis returned " << flat.size() << " elements\n";
    for (auto& s : flat) std::cerr << "  " << s << "\n";

    std::vector<Score> result;
    flat = std::get<0>(resp).value();
    for (size_t i = 0; i + 1 < flat.size(); i += 2) {
        result.emplace_back(flat[i], std::stoi(flat[i + 1]), 0);
    }
    co_return result;
}

net::awaitable<Leaderboard> RedisService::create_or_get_leaderboard(const std::string& name) {
    redis::request req;
    req.push("HGET", "leaderboards:by_name", name);
    
    redis::response<std::optional<std::string>> resp;
    co_await _redis_connection->async_exec(req, resp, net::use_awaitable);
    
    auto& existing_id = std::get<0>(resp).value();
    if (existing_id.has_value()) {
        std::cerr << "Leaderboard with name '" << name << "' already exists with ID: " << *existing_id << "\n";
        co_return Leaderboard(*existing_id, name);
    }
    
    std::string new_uuid = boost::uuids::to_string(boost::uuids::random_generator{}());
    std::cerr << "Creating new leaderboard '" << name << "' with ID: " << new_uuid << "\n";
    
    redis::request create_req;
    create_req.push("MULTI");
    create_req.push("HSET", "leaderboards:by_name", name, new_uuid);
    create_req.push("HSET", "leaderboard:" + new_uuid + ":meta", "name", name);
    create_req.push("EXEC");
    
    redis::response<
        redis::ignore_t,  // MULTI
        redis::ignore_t,  // HSET
        redis::ignore_t,  // HSET
        redis::ignore_t   // EXEC
    > create_resp;
    
    boost::system::error_code ec;
    co_await _redis_connection->async_exec(create_req, create_resp, net::redirect_error(net::use_awaitable, ec));
    if (ec) {
        std::cerr << "Redis exec error creating leaderboard: " << ec.message() << std::endl;
        throw boost::system::system_error(ec);
    }
    
    co_return Leaderboard(new_uuid, name);
}

net::awaitable<std::optional<std::string>> RedisService::get_leaderboard_id_by_name(const std::string& name) {
    redis::request req;
    req.push("HGET", "leaderboards:by_name", name);
    
    redis::response<std::optional<std::string>> resp;
    co_await _redis_connection->async_exec(req, resp, net::use_awaitable);
    
    co_return std::get<0>(resp).value();
}

net::awaitable<std::optional<Leaderboard>> RedisService::get_leaderboard_by_id(const std::string& id) {
    redis::request req;
    req.push("HGET", "leaderboard:" + id + ":meta", "name");
    
    redis::response<std::optional<std::string>> resp;
    co_await _redis_connection->async_exec(req, resp, net::use_awaitable);
    
    auto& name = std::get<0>(resp).value();
    if (!name.has_value()) {
        co_return std::nullopt;
    }
    
    co_return Leaderboard(id, *name);
}
