#include "Habit.h"
#include "Utils.h"
#include <iostream>
#include <limits>
#include <windows.h> // Required for Sleep()

using namespace std;

// --- HABIT BASE CLASS ---
Habit::Habit(int i, string n, int xp) : id(i), name(n), isCompleted(false), baseXP(xp) {}
int Habit::getId() const { return id; }
string Habit::getName() const { return name; }
bool Habit::getStatus() const { return isCompleted; }
int Habit::getBaseXP() const { return baseXP; }

// --- BINARY HABIT ---
BinaryHabit::BinaryHabit(int i, string n, int xp) : Habit(i, n, xp) {}

void BinaryHabit::display() const {
    cout << (isCompleted ? GREEN : YELLOW) << "[" << id << "] " 
         << (isCompleted ? "[X] " : "[ ] ") << name 
         << (isCompleted ? " (Completed)" : " (+ " + to_string(baseXP) + " XP)") 
         << RESET << "\n";
}

bool BinaryHabit::logCompletion() {
    if (isCompleted) {
        cout << YELLOW << "Already completed today.\n" << RESET;
        return false;
    }
    isCompleted = true;
    cout << GREEN << "Completed: " << name << RESET << "\n";
    return true;
}

// --- SLIDER HABIT ---
SliderHabit::SliderHabit(int i, string n, int xp, int target, string u) 
    : Habit(i, n, xp), currentAmount(0), targetAmount(target), unit(u) {}

void SliderHabit::display() const {
    cout << (isCompleted ? GREEN : CYAN) << "[" << id << "] " << name 
         << " (" << currentAmount << "/" << targetAmount << " " << unit << ")" 
         << (isCompleted ? " Completed" : "") << RESET << "\n";
}

bool SliderHabit::logCompletion() {
    if (isCompleted) {
        cout << YELLOW << "Target already reached today.\n" << RESET;
        return false;
    }
    int n;
    cout << "How many " << unit << " did you complete? ";
    if (!(cin >> n) || n <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << RED << "Enter a positive number.\n" << RESET;
        return false;
    }
    clearInputBuffer();
    currentAmount += n;
    renderProgressBar(currentAmount, targetAmount, CYAN);
    
    if (currentAmount >= targetAmount) {
        currentAmount = targetAmount;
        isCompleted = true;
        cout << GREEN << "Target reached!\n" << RESET;
        return true;
    }
    cout << "Keep going: " << targetAmount - currentAmount << " " << unit << " remaining.\n";
    return false;
}

// --- TIMER HABIT ---
TimerHabit::TimerHabit(int i, string n, int xp, int mins) 
    : Habit(i, n, xp), targetMinutes(mins) {}

void TimerHabit::display() const {
    cout << (isCompleted ? GREEN : MAGENTA) << "[" << id << "] " << name 
         << " (" << targetMinutes << " minutes)" 
         << (isCompleted ? " Completed" : "") << RESET << "\n";
}

bool TimerHabit::logCompletion() {
    if (isCompleted) {
        cout << "Already completed today.\n";
        return false;
    }
    char c;
    cout << "Start a " << targetMinutes << " minute focus timer? (y/n): ";
    cin >> c;
    clearInputBuffer();
    
    if (c != 'y' && c != 'Y') return false;
    
    cout << "Timer running. Keep this window open...\n";
    for (int s = targetMinutes * 60; s > 0; --s) {
        cout << "\rRemaining: " << s / 60 << "m " << s % 60 << "s   " << flush;
        Sleep(1000); 
    }
    cout << "\n" << GREEN << "Timer complete!\n" << RESET;
    isCompleted = true;
    return true;
}