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
    float _current_score;
    time_t _last_updated;
    std::vector<uid_t> _friends_list;
    
public:
    Player(uid_t uid, const std::string& username)
        : _uid(uid), _username(username), _current_score(0), _last_updated(std::time(nullptr)) {}
    uid_t uid() const { return _uid; }
    void uid(uid_t uid) { _uid = uid; }
    std::string username() const { return _username; }
    void username(const std::string& username) { _username = username; }
    float current_score() const { return _current_score; }
    void current_score(float score) { _current_score = score; }
    time_t last_updated() const { return _last_updated; }
    void last_updated(time_t timestamp) { _last_updated = timestamp; }
    std::vector<uid_t> friends_list() const { return _friends_list; }
    void add_friend(uid_t friend_uid);
    void remove_friend(uid_t friend_uid);
    boost::json::object to_json() const;
};

#endif // PLAYER_H
