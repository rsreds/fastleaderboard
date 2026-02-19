#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <vector>
#include "player.h"
#include "score.h"

class Leaderboard
{
private:
    std::vector<Score> _scores;

public:
    Leaderboard(/* args */);
    std::vector<Score> scores() const { return _scores; }
    void add_score(const Score& score) { _scores.push_back(score); }
    void remove_score(const Score& score) {
        _scores.erase(std::remove(_scores.begin(), _scores.end(), score), _scores.end());
    }
};

Leaderboard::Leaderboard(/* args */)
{
}

#endif // LEADERBOARD_H
