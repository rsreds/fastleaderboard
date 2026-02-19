#include "score.h"

boost::json::object Score::to_json() const
{
    boost::json::object obj;
    obj["player_id"] = _player_uuid;
    obj["score"] = _score;
    obj["timestamp"] = _timestamp;
    return obj;
}

std::string Score::to_redis_member() const {
    return std::to_string(_score) + ":" + std::to_string(_timestamp);
}

bool operator==(const Score& lhs, const Score& rhs) {
    return lhs._player_uuid == rhs._player_uuid && lhs._score == rhs._score && lhs._timestamp == rhs._timestamp;
}
