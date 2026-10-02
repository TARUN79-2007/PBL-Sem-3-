#include "Habit.h"
#include "Utils.h"
#include <iostream>
#include <windows.h>

using namespace std;
// Data struct to pass variables into the thread safely
struct TimerData {
    int minutes;
    string taskName;
};

// The background thread function for the timer
DWORD WINAPI LiveTimerThread(LPVOID lpParam) {
    TimerData* data = (TimerData*)lpParam;
    int totalSeconds = data->minutes * 60;
    
    // Grab the console handle to manipulate the cursor
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    
    while (totalSeconds > 0) {
        // Jump to the top right corner (X: 50, Y: 1)
        coord.X = 50; coord.Y = 1;
        SetConsoleCursorPosition(hConsole, coord);
        
        cout << MAGENTA << "[TIMER: " << data->taskName << " - " 
             << totalSeconds / 60 << "m " << (totalSeconds % 60) << "s]   " << RESET;
        
        Sleep(1000);
        totalSeconds--;
    }

    // Timer finished message
    coord.X = 50; coord.Y = 1;
    SetConsoleCursorPosition(hConsole, coord);
    cout << GREEN << "[TIMER: " << data->taskName << " COMPLETE!]          " << RESET;
    
    delete data; // Prevent memory leaks
    return 0;
}

// ==========================================
// BASE CLASS: Habit Implementation
// ==========================================
Habit::Habit(int _id, string _name, int _baseXP) 
    : id(_id), name(_name), baseXP(_baseXP), isCompleted(false) {}

int Habit::getId() const { return id; }
string Habit::getName() const { return name; }
bool Habit::getStatus() const { return isCompleted; }
int Habit::getBaseXP() const { return baseXP; }


// ==========================================
// DERIVED CLASS 1: BinaryHabit (Yes/No)
// ==========================================
BinaryHabit::BinaryHabit(int _id, string _name, int _baseXP)
    : Habit(_id, _name, _baseXP) {}

void BinaryHabit::display() const {
    if (isCompleted) {
        cout << GREEN << "[" << id << "] [X] " << name << " (Completed)" << RESET << "\n";
    } else {
        cout << YELLOW << "[" << id << "] [ ] " << name << " (+ " << baseXP << " XP)" << RESET << "\n";
    }
}

bool BinaryHabit::logCompletion() {
    if (isCompleted) {
        cout << YELLOW << "You already completed this habit today!\n" << RESET;
        return false;
    }
    
    isCompleted = true;
    cout << GREEN << "Awesome! You completed: " << name << RESET << "\n";
    return true; // Returns true so the Gamification Engine knows to award XP
}


// ==========================================
// DERIVED CLASS 2: SliderHabit (Quantity)
// ==========================================
SliderHabit::SliderHabit(int _id, string _name, int _baseXP, int _targetAmount, string _unit)
    : Habit(_id, _name, _baseXP), currentAmount(0), targetAmount(_targetAmount), unit(_unit) {}

void SliderHabit::display() const {
    if (isCompleted) {
        cout << GREEN << "[" << id << "] [X] " << name << " (" << targetAmount << "/" << targetAmount << " " << unit << ")" << RESET << "\n";
    } else {
        cout << CYAN << "[" << id << "] [-] " << name << " (" << currentAmount << "/" << targetAmount << " " << unit << ")" << RESET << "\n";
    }
}

bool SliderHabit::logCompletion() {
    if (isCompleted) {
        cout << YELLOW << "Target already reached for today!\n" << RESET;
        return false;
    }

    int addedAmount;
    cout << "How many " << unit << " did you just complete? ";
    cin >> addedAmount;

    clearInputBuffer();

    if (addedAmount <= 0) {
        cout << RED << "Invalid amount.\n" << RESET;
        return false;
    }

    currentAmount += addedAmount;

    // Use our native Utils function to show a cool progress bar!
    renderProgressBar(currentAmount, targetAmount, CYAN);

    if (currentAmount >= targetAmount) {
        currentAmount = targetAmount; // Cap it
        isCompleted = true;
        cout << GREEN << "\nTarget Reached! Great job!" << RESET << "\n";
        return true; // Award XP
    } else {
        cout << YELLOW << "\nKeep going! You have " << (targetAmount - currentAmount) << " " << unit << " left.\n" << RESET;
        return false; // Not fully complete yet, no base XP awarded yet
    }
}
// ==========================================
// DERIVED CLASS 3: TimerHabit (Duration)
// ==========================================
TimerHabit::TimerHabit(int _id, string _name, int _baseXP, int _targetMinutes)
    : Habit(_id, _name, _baseXP), targetMinutes(_targetMinutes) {}

void TimerHabit::display() const {
    if (isCompleted) {
        cout << GREEN << "[" << id << "] [X] " << name << " (" << targetMinutes << " mins completed)" << RESET << "\n";
    } else {
        cout << MAGENTA << "[" << id << "] [~] " << name << " (" << targetMinutes << " mins focus required)" << RESET << "\n";
    }
}

bool TimerHabit::logCompletion() {
    if (isCompleted) {
        cout << YELLOW << "You already completed this focus session today!\n" << RESET;
        return false;
    }

    cout << "Start background timer for " << targetMinutes << " minutes? (y/n): ";
    char choice;
    cin >> choice;
    
    // Clear the input buffer so it doesn't mess up the menu later
    while(cin.get() != '\n'); 

    if (choice == 'y' || choice == 'Y') {
        cout << CYAN << "\nTimer launched! You can keep using the menu.\n" << RESET;
        
        TimerData* tData = new TimerData{targetMinutes, name};
        CreateThread(NULL, 0, LiveTimerThread, tData, 0, NULL);
        
        isCompleted = true; 
        return true; 
    } else {
        cout << YELLOW << "Timer cancelled.\n" << RESET;
        return false;
    }
}