#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/json.hpp>

class Player
{
private:
    std::string _uuid;
    std::string _username;
    std::vector<uid_t> _friends_list; // Redis SET: players:{uid}:friends
    
public:
    explicit Player(std::string username)
        : _uuid(boost::uuids::to_string(boost::uuids::random_generator{}()))
        , _username(std::move(username))
    {}

    Player(std::string uuid, std::string username)
        : _uuid(std::move(uuid)), _username(std::move(username)) 
    {}

    std::string uuid() const { return _uuid; }
    std::string username() const { return _username; }
    std::vector<uid_t> friends_list() const { return _friends_list; }
    void add_friend(uid_t friend_uid);
    void remove_friend(uid_t friend_uid);
    boost::json::object to_json() const;
};

#endif // PLAYER_H
