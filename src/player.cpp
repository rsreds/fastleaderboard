#include "player.h"
#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>



void Player::add_friend(uid_t friend_uid)
{
    if (std::find(_friends_list.begin(), _friends_list.end(), friend_uid) == _friends_list.end()) {
        _friends_list.push_back(friend_uid);
    }
}

void Player::remove_friend(uid_t friend_uid)
{
    _friends_list.erase(std::remove(_friends_list.begin(), _friends_list.end(), friend_uid), _friends_list.end());
}

boost::json::object Player::to_json() const
{
    boost::json::object obj;
    obj["player_id"] = _uuid;
    obj["username"] = _username;
    boost::json::array friends_array;
    for (const auto& friend_uid : _friends_list) {
        friends_array.push_back(friend_uid);
    }
    obj["friends_list"] = friends_array;
    return obj;
}
