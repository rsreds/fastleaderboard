#include "score.h"

boost::json::object Score::to_json() const
{
    boost::json::object obj;
    obj["player_id"] = _player_id;
    obj["score"] = _score;
    obj["timestamp"] = _timestamp;
    return obj;
}
