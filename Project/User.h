#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    std::string username;
    int level;
    int totalXP;
    int hearts;
    int currentStreak;

public:
    // Constructor initializes a new player with starter stats
    User(std::string name) : username(name), level(1), totalXP(0), hearts(5), currentStreak(0) {}

    // Getters
    std::string getUsername() const { return username; }
    int getLevel() const { return level; }
    int getXP() const { return totalXP; }
    int getHearts() const { return hearts; }
    int getStreak() const { return currentStreak; }

    // Setters (Used by the Gamification Engine and Database)
    void addXP(int amount) { totalXP += amount; }
    void setLevel(int newLevel) { level = newLevel; }
    void setXP(int newXP) { totalXP = newXP; }
    void setHearts(int newHearts) { hearts = newHearts; }
    void setStreak(int newStreak) { currentStreak = newStreak; }
    
    void addStreak() { currentStreak++; }
    void resetStreak() { currentStreak = 0; }
    void deductHeart(int amount) { hearts -= amount; if(hearts < 0) hearts = 0; }
    void restoreHearts() { hearts = 5; }
};

#endif