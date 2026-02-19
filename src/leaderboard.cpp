#include "leaderboard.h"

uid_t Leaderboard::get_player_id(const std::string& player_name) const {
    auto it = std::find_if(_players.begin(), _players.end(), [&player_name](const Player& player) {
        return player.username() == player_name;
    });
    if (it != _players.end()) {
        return it->uid();
    }
    return 0; // Return 0 if player not found
}

std::vector<Player> Leaderboard::get_top_players(std::size_t n) const {
    std::vector<Player> sorted_players = _players;
    std::sort(sorted_players.begin(), sorted_players.end(), [](const Player& a, const Player& b) {
        return a.current_score() > b.current_score(); // Sort in descending order
    });
    if (n > sorted_players.size()) {
        n = sorted_players.size();
    }
    return std::vector<Player>(sorted_players.begin(), sorted_players.begin() + n);
}

std::vector<Score> Leaderboard::get_top_scores(std::size_t n) const {
    std::vector<Score> sorted_scores = _scores;
    std::sort(sorted_scores.begin(), sorted_scores.end(), [](const Score& a, const Score& b) {
        return a.score() > b.score(); // Sort in descending order
    });
    if (n > sorted_scores.size()) {
        n = sorted_scores.size();
    }
    return std::vector<Score>(sorted_scores.begin(), sorted_scores.begin() + n);
}

std::vector<Score> Leaderboard::get_player_scores(const uid_t player_id, std::size_t n) const {
    std::vector<Score> player_scores;
    for (const auto& score : _scores) {
        if (score.player_id() == player_id) {
            player_scores.push_back(score);
        }
    }
    std::sort(player_scores.begin(), player_scores.end(), [](const Score& a, const Score& b) {
        return a.score() > b.score(); // Sort in descending order
    });
    if (n > player_scores.size()) {
        n = player_scores.size();
    }
    return std::vector<Score>(player_scores.begin(), player_scores.begin() + n);
}

std::vector<Score> Leaderboard::get_players_in_range(const uid_t player_id, std::size_t n) const {
    std::vector<Score> sorted_scores = _scores;
    std::sort(sorted_scores.begin(), sorted_scores.end(), [](const Score& a, const Score& b) {
        return a.score() > b.score(); // Sort in descending order
    });
    auto it = std::find_if(sorted_scores.begin(), sorted_scores.end(), [player_id](const Score& score) {
        return score.player_id() == player_id;
    });
    if (it == sorted_scores.end()) {
        return {}; // Player not found
    }
    std::vector<Score> range_scores;
    auto start_it = (it >= sorted_scores.begin() + n) ? it - n : sorted_scores.begin();
    auto end_it = (it + n < sorted_scores.end()) ? it + n + 1 : sorted_scores.end();
    range_scores.insert(range_scores.end(), start_it, end_it);
    return range_scores;
}

void Leaderboard::add_score(const Score& score){
    _scores.push_back(score);
    //if new score is better than player's previous best, update player's current score and timestamp
    auto it = std::find_if(_players.begin(), _players.end(), [&score](const Player& player) {
        return player.uid() == score.player_id();
    });
    if (it != _players.end()) {
        if (score.score() > it->current_score()) {
            it->current_score(score.score());
        }
        it->last_updated(score.timestamp());
    }
}

void Leaderboard::submit_score(std::string player_username, float score, time_t timestamp) {
    //if player is new, add to players list
    auto it = std::find_if(_players.begin(), _players.end(), [&player_username](const Player& player) {
        return player.username() == player_username;
    });
    uid_t player_id;
    if (it == _players.end()) {
        player_id = _players.size() + 1; // Simple way to generate player ID
        _players.emplace_back(player_id, player_username);
    } else {
        player_id = it->uid();
    }
    Score new_score(player_id, score, timestamp);
    add_score(new_score);
}