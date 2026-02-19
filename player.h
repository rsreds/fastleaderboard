#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <boost/json.hpp>

class Player
{
private:
    uid_t _uid;
    std::string _username;
    std::vector<uid_t> _friends_list;
    
public:
    Player(){};
    uid_t uid() const { return _uid; }
    void uid(uid_t uid) { _uid = uid; }
    std::string username() const { return _username; }
    void username(const std::string& username) { _username = username; }
    std::vector<uid_t> friends_list() const { return _friends_list; }
    void add_friend(uid_t friend_uid);
    void remove_friend(uid_t friend_uid);
    boost::json::object to_json() const;
};

#endif // PLAYER_H
