#include "session.h"
#include "parsed_request.h"
#include "json_helpers.h"

#include <iostream>

void Session::do_read() {
    auto self(shared_from_this());
    http::async_read(stream_, buffer_, req_,
        [this, self](beast::error_code ec, std::size_t bytes_transferred) {
            boost::ignore_unused(bytes_transferred);
            if (!ec)
                on_read(ec, bytes_transferred);
        });
}

Response handle_request(const Request& req, Leaderboard& lb) {
    auto ver = req.version();
    auto ka  = req.keep_alive();
    auto pr  = parse_request(req);

    using namespace json_helpers;

    if (pr.method == "OPTIONS") {
        Response res{http::status::no_content, ver};
        res.keep_alive(ka);
        res.set(http::field::access_control_allow_origin, "*");
        res.set(http::field::access_control_allow_methods, "GET, POST, DELETE, OPTIONS");
        res.set(http::field::access_control_allow_headers, "Content-Type");
        res.prepare_payload();
        return res;
    }
    else if (pr.method == "GET") {
        if (pr.path == "/leaderboard/top") {
            std::size_t n = 10; // Default value
            auto it = pr.query_params.find("n");
            if (it != pr.query_params.end()) {
                try {
                    n = static_cast<std::size_t>(std::stoul(it->second));
                } catch (const std::exception& e) {
                    return make_error_response(ver, ka, http::status::bad_request, 
                        "Invalid 'n' parameter: " + std::string(e.what()));
                }
            }
            n = std::min(n, static_cast<std::size_t>(100));
            auto top_scores = lb.get_top_scores(n);
            return make_json_response(ver, ka, wrap_array("top_scores", to_json_array(top_scores)));
        }

        if (pr.path == "/leaderboard/top_players") {
            std::size_t n = 10; // Default value
            auto it = pr.query_params.find("n");
            if (it != pr.query_params.end()) {
                try {
                    n = static_cast<std::size_t>(std::stoul(it->second));
                } catch (const std::exception& e) {
                    return make_error_response(ver, ka, http::status::bad_request,
                        "Invalid 'n' parameter: " + std::string(e.what()));
                }
            }
            n = std::min(n, static_cast<std::size_t>(100));
            auto top_players = lb.get_top_players(n);
            return make_json_response(ver, ka, wrap_array("top_players", to_json_array(top_players)));
        }

        if (pr.path == "/leaderboard/player"){
            auto it = pr.query_params.find("name");
            if (it == pr.query_params.end()) {
                return make_error_response(ver, ka, http::status::bad_request, "Missing 'name' parameter");
            }
            std::string player_name = it->second;
            uid_t player_id = lb.get_player_id(player_name);
            if (player_id == 0) {
                return make_error_response(ver, ka, http::status::not_found, "Player not found");
            }
            std::size_t n = 10; // Default value
            it = pr.query_params.find("n");
            if (it != pr.query_params.end()) {
                try {
                    n = static_cast<std::size_t>(std::stoul(it->second));
                } catch (const std::exception& e) {
                    return make_error_response(ver, ka, http::status::bad_request,
                        "Invalid 'n' parameter: " + std::string(e.what()));
                }
            }
            n = std::min(n, static_cast<std::size_t>(100));
            auto player_scores = lb.get_player_scores(player_id, n);
            
            // Use to_json_minimal for scores without player_id
            boost::json::array scores_array;
            for (const auto& score : player_scores) {
                scores_array.push_back(score.to_json().erase("player_id"));
            }
            boost::json::object response;
            response["player_id"] = player_id;
            response["scores"] = scores_array;
            return make_json_response(ver, ka, response);
        }
    }

    else if (pr.method == "POST") {
        if (pr.path == "/leaderboard/submit") {
            try {
                auto json_body = boost::json::parse(pr.body).as_object();
                std::string player_name = json_body.at("player_name").as_string().c_str();
                float score = static_cast<float>(json_body.at("score").as_double());
                time_t timestamp = std::time(nullptr);
                lb.submit_score(player_name, score, timestamp);
                return make_json_response(ver, ka, {{"message", "Score submitted successfully"}});
            } catch (const std::exception& e) {
                return make_error_response(ver, ka, http::status::bad_request, 
                    "Invalid request body: " + std::string(e.what()));
            }
        }
    }

    else if (pr.method == "DELETE") {
    }

    return make_error_response(ver, ka, http::status::bad_request, "Unsupported HTTP method");
}


void Session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    if (ec == http::error::end_of_stream)
        return do_close();
    if (ec) return; // Handle error

    auto res = std::make_shared<Response>(handle_request(req_, lb));

    auto self(shared_from_this());
    http::async_write(stream_, *res,
        [self, res](beast::error_code ec, std::size_t) {
            if (ec) return; // Handle error
            if (res->keep_alive())
                self->do_read();
            else
                self->do_close();
        });
}


void Session::do_close() {
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
}