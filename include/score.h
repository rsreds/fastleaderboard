#ifndef SCORE_H
#define SCORE_H

#include <string>
#include <chrono>
#include <boost/json.hpp>

class Score
{
private:
    uid_t _player_id;
    float _score;
    time_t _timestamp;

public:
    Score(uid_t player_id, float score, time_t timestamp)
        : _player_id(player_id), _score(score), _timestamp(timestamp) {};
    uid_t player_id() const { return _player_id; }
    void player_id(uid_t player_id) { _player_id = player_id; }
    float score() const { return _score; }
    void score(float score) { _score = score; }
    time_t timestamp() const { return _timestamp; }
    void timestamp(time_t timestamp) { _timestamp = timestamp; }
    friend bool operator==(const Score& lhs, const Score& rhs) {
        return lhs._player_id == rhs._player_id && lhs._score == rhs._score && lhs._timestamp == rhs._timestamp;
    }
    boost::json::object to_json() const;
    // Returns JSON without player_id (for player-specific endpoints)
    boost::json::object to_json_minimal() const;
};

#endif // SCORE_H
