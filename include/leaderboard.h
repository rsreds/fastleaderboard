#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <vector>
#include "player.h"
#include "score.h"

class Leaderboard
{
private:
    std::vector<Score> _scores;
    std::vector<Player> _players;
    void add_score(const Score& score);

public:
    Leaderboard() = default;
    std::vector<Score> scores() const { return _scores; }
    std::vector<Player> players() const { return _players; }
    void submit_score(const Score& score);
    void submit_score(std::string player_name, float score, time_t timestamp);
    void remove_player(const uid_t player_id);
    uid_t get_player_id(const std::string& player_name) const;
    std::vector<Player> get_top_players(std::size_t n) const;
    std::vector<Score> get_top_scores(std::size_t n) const;
    std::vector<Score> get_player_scores(const uid_t player_id, std::size_t n) const;
    std::vector<Score> get_players_in_range(const uid_t player_id, std::size_t n) const;
};

#endif // LEADERBOARD_H
