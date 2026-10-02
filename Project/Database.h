#ifndef DATABASE_H
#define DATABASE_H

#include "User.h"
#include "Habit.h"
#include <vector>
#include <string>

class Database {
public:
    // User State Persistence
    static bool saveUserState(const User& u, const std::string& filepath = "user_profile.dat");
    static bool loadUserState(User& u, const std::string& filepath = "user_profile.dat");

    // Habit List Persistence (Polymorphic Serialization)
    static bool saveHabits(const std::vector<Habit*>& habits, const std::string& filepath = "habits.dat");
    static bool loadHabits(std::vector<Habit*>& habits, const std::string& filepath = "habits.dat");
};

#endif