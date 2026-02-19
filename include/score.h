#ifndef SCORE_H
#define SCORE_H

#include <string>
#include <chrono>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/json.hpp>

class Score
{
private:
    std::string _player_uuid;
    int _score;
    time_t _timestamp;
    // Redis member string: "{score}:{timestamp}" 
    // Redis key: leaderboard:{lb_id}:player:{player_id}

public:
    Score(std::string player_uuid, int score, time_t timestamp)
        : _player_uuid(std::move(player_uuid))
        , _score(score)
        , _timestamp(timestamp)
    {}

    const std::string& player_uuid() const { return _player_uuid; }
    int score() const { return _score; }
    time_t timestamp() const { return _timestamp; }

    friend bool operator==(const Score&, const Score&);
    boost::json::object to_json() const;
    std::string to_redis_member() const;

};

#endif // SCORE_H
