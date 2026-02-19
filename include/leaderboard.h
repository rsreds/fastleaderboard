#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

class Leaderboard
{
private:
    std::string _leaderboard_uuid;
    std::string _name;
    // Redis keys:
    // leaderboard:{id}          → ZSET (ranking, best scores)
    // leaderboard:{id}:history:{player_id} → ZSET (all submissions)

public:
    Leaderboard(std::string name) 
        : _leaderboard_uuid(boost::uuids::to_string(boost::uuids::random_generator{}()))
        , _name(std::move(name))
    {}

    Leaderboard(std::string uuid, std::string name) 
        : _leaderboard_uuid(std::move(uuid))
        , _name(std::move(name))
    {}

    const std::string& id() const { return _leaderboard_uuid; }
    const std::string& name() const { return _name; }
};

#endif // LEADERBOARD_H
