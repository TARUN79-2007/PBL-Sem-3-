#include "GamificationEngine.h"
#include "Utils.h"
#include <iostream>
#include <cmath>
#include <windows.h> // Native Windows sleep

using namespace std;

void GamificationEngine::awardXP(User& u, int baseXP) {
    // 1. Calculate bonus based on current streak
    int streakBonus = u.getStreak() * 2; 
    int totalEarned = baseXP + streakBonus;

    // 2. Add to user profile
    u.addXP(totalEarned);
    u.addStreak();

    // 3. UI Feedback
    cout << GREEN << "\nTask Completed! " << RESET;
    cout << YELLOW << "+" << totalEarned << " XP " << RESET;
    cout << "(Base: " << baseXP << " | Streak Bonus: " << streakBonus << ")\n";
    
    Sleep(800); // Replaced std::this_thread

    // 4. Check if they hit the threshold for a new level
    checkLevelUp(u);
}

void GamificationEngine::checkLevelUp(User& u) {
    // RPG Leveling Curve Formula: Level = sqrt(XP / 10) + 1
    int calculatedLevel = static_cast<int>(std::sqrt(u.getXP() / 10.0)) + 1;

    if (calculatedLevel > u.getLevel()) {
        u.setLevel(calculatedLevel);
        
        // Level Up Animation
        clearScreen();
        cout << CYAN << "========================================\n" << RESET;
        cout << YELLOW << "           LEVEL UP! \n" << RESET;
        cout << CYAN << "========================================\n" << RESET;
        cout << GREEN << u.getUsername() << " is now Level " << u.getLevel() << "!\n" << RESET;
        cout << "You are getting stronger. Keep up the consistency!\n\n";
        
        // Fully restore hearts on level up 
        if (u.getHearts() < 5) {
            u.restoreHearts();
            cout << RED << "All 5 Hearts Restored!\n" << RESET;
        }
        
        cout << "Press Enter to continue...";
        cin.get(); // Wait for user input
    }
}

void GamificationEngine::evaluatePenalties(User& u, int missedDays) {
    if (missedDays <= 0) return; // No penalty if they logged in daily

    cout << RED << "\nWARNING: You missed " << missedDays << " day(s)!\n" << RESET;
    
    // Deduct hearts
    u.deductHeart(missedDays);
    cout << "You lost " << missedDays << " Heart(s). Current Hearts: " << u.getHearts() << "/5\n";

    // Streak penalty logic
    if (u.getHearts() == 0) {
        cout << RED << "Your health has reached 0. Your streak has been shattered!\n" << RESET;
        u.resetStreak();
        // Give them back basic hearts so they aren't permanently stuck at 0
        u.restoreHearts(); 
        cout << CYAN << "The system has mercifully restored 5 hearts. Try again.\n" << RESET;
    } else {
        cout << YELLOW << "Your streak of " << u.getStreak() << " is hanging by a thread...\n" << RESET;
    }
    
    Sleep(2000); // Replaced std::this_thread
}