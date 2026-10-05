#include "Database.h"
#include <fstream>
#include <iostream>
#include <limits>
using namespace std;

// ==========================================
// USER STATE SERIALIZATION
// ==========================================
bool Database::saveUserState(const User& u, const string& filepath) {
    ofstream outFile(filepath);
    if (!outFile.is_open()) return false;

    outFile << u.getUsername() << "\n";
    outFile << u.getLevel() << "\n";
    outFile << u.getXP() << "\n";
    outFile << u.getHearts() << "\n";
    outFile << u.getStreak() << "\n";

    outFile.close();
    return true;
}

bool Database::loadUserState(User& u, const string& filepath) {
    ifstream inFile(filepath);
    if (!inFile.is_open()) return false; 

    string name;
    int level, xp, hearts, streak;

    getline(inFile, name); 
    if (inFile >> level >> xp >> hearts >> streak) {
        u.setLevel(level);
        u.setXP(xp);
        u.setHearts(hearts);
        u.setStreak(streak);
    }

    inFile.close();
    return true;
}

// ==========================================
// HABIT SERIALIZATION (Polymorphic RTTI)
// ==========================================
bool Database::saveHabits(const vector<Habit*>& habits, const string& filepath) {
    ofstream outFile(filepath);
    if (!outFile.is_open()) return false;

    outFile << habits.size() << "\n";

    for (const Habit* h : habits) {
        // Use RTTI (dynamic_cast) to determine the derived class type at runtime!
        int typeId = 1; // Default to BinaryHabit
        if (dynamic_cast<const SliderHabit*>(h)) typeId = 2;
        else if (dynamic_cast<const TimerHabit*>(h)) typeId = 3;

        outFile << typeId << "\n"; 
        outFile << h->getId() << "\n";
        outFile << h->getName() << "\n";
        outFile << h->getBaseXP() << "\n";
        outFile << h->getStatus() << "\n";
    }

    outFile.close();
    return true;
}

bool Database::loadHabits(vector<Habit*>& habits, const string& filepath) {
    ifstream inFile(filepath);
    if (!inFile.is_open()) return false;

    int habitCount;
    if (!(inFile >> habitCount)) return false;

    // Clean up existing pointers
    for(Habit* h : habits) {
        delete h;
    }
    habits.clear();

    for (int i = 0; i < habitCount; ++i) {
        int typeId, id, baseXP;
        bool status;
        string name;

        if (!(inFile >> typeId >> id)) {
    return false;
}

inFile.ignore(
    numeric_limits<streamsize>::max(), '\n'
);

if (!getline(inFile, name)) {
    return false;
}

if (!(inFile >> baseXP >> status)) {
    return false;
}

        // Reconstruct the correct derived object based on the typeId
        Habit* newHabit = nullptr;
        if (typeId == 1) {
            newHabit = new BinaryHabit(id, name, baseXP);
        } else if (typeId == 2) {
            // Using standard fallback targets for saved Slider tasks
            newHabit = new SliderHabit(id, name, baseXP, 50, "units"); 
        } else if (typeId == 3) {
            // Using standard fallback minutes for saved Timer tasks
            newHabit = new TimerHabit(id, name, baseXP, 30); 
        }

        if (newHabit) {
            // Silently mark as done without triggering the UI prompts!
            if (status) newHabit->setStatus(true); 
            habits.push_back(newHabit);
        }
    }

    inFile.close();
    return true;
}