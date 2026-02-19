#ifndef SCORE_H
#define SCORE_H

#include <string>
#include <chrono>

class Score
{
private:
    uid_t _uid;
    float _score;
    time_t _timestamp;

public:
    Score(/* args */);
    uid_t uid() const { return _uid; }
    void uid(uid_t uid) { _uid = uid; }
    float score() const { return _score; }
    void score(float score) { _score = score; }
    time_t timestamp() const { return _timestamp; }
    void timestamp(time_t timestamp) { _timestamp = timestamp; }
    friend bool operator==(const Score& lhs, const Score& rhs) {
        return lhs._uid == rhs._uid && lhs._score == rhs._score && lhs._timestamp == rhs._timestamp;
    }
};

Score::Score(/* args */)
{
}

#endif // SCORE_H
