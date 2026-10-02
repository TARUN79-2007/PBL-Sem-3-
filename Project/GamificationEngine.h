#ifndef GAMIFICATION_ENGINE_H
#define GAMIFICATION_ENGINE_H

#include "User.h"

class GamificationEngine {
private:
    // Helper function to handle the math of leveling up
    static void checkLevelUp(User& u);

public:
    // Core game mechanics
    static void awardXP(User& u, int baseXP);
    static void evaluatePenalties(User& u, int missedDays);
};

#endif