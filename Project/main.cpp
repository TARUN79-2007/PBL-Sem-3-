#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <conio.h>

#include "Utils.h"
#include "User.h"
#include "GamificationEngine.h"
#include "Habit.h"
#include "Database.h"
#include "Leaderboard.h"

using namespace std;

// --- UI Helper Functions ---
void moveCursor(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setCursorVisibility(bool visible) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}
// --- BACKGROUND THREADS ---
DWORD WINAPI LaunchAnalytics(LPVOID lpParam) {
    system("python analytics.py");
    return 0;
}
// --- REUSABLE INTERACTIVE MENU ENGINE ---
int displayInteractiveMenu(const string& title, const vector<string>& options) {
    int selectedIndex = 0;
    bool isChoosing = true;
    
    setCursorVisibility(false); // Hide cursor during navigation
    clearScreen();              // Clear the screen once before drawing the menu

    while (isChoosing) {
        moveCursor(0, 0); // Lock cursor to top-left to prevent flicker

        cout << CYAN << "====================================================\n" << RESET;
        cout << GREEN << "  " << title << "  \n" << RESET;
        cout << CYAN << "====================================================\n\n" << RESET;
        
        for (int i = 0; i < options.size(); ++i) {
            if (i == selectedIndex) {
                // Cyan highlight for selection
                cout << "\x1b[46;30m > " << options[i] << " \x1b[0m                                       \n";
            } else {
                cout << "   " << options[i] << "                                         \n";
            }
        }
        cout << "\nUse UP/DOWN arrows to navigate, ENTER to select.              \n";

        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72) { // UP
                selectedIndex--;
                if (selectedIndex < 0) selectedIndex = options.size() - 1;
            } else if (key == 80) { // DOWN
                selectedIndex++;
                if (selectedIndex >= options.size()) selectedIndex = 0;
            }
        } else if (key == 13) { // ENTER
            isChoosing = false;
        }
    }
    setCursorVisibility(true); // Bring cursor back for normal inputs
    clearScreen();
    return selectedIndex;
}

// Display the main UI Dashboard
void displayDashboard(User& u, const vector<Habit*>& habits) {
    cout << CYAN << "========================================\n" << RESET;
    cout << YELLOW << "          HABITQUEST DASHBOARD\n" << RESET;
    cout << CYAN << "========================================\n\n" << RESET;

    cout << "User: " << GREEN << u.getUsername() << RESET << " | Level: " << YELLOW << u.getLevel() << RESET << "\n";
    cout << "Streak: " << MAGENTA << u.getStreak() << " Days \xE2\x94\x82 " << RESET; 
    
    cout << "Health: " << RED;
    for(int i = 0; i < 5; i++) {
        if(i < u.getHearts()) cout << "<3 ";
        else cout << "- ";
    }
    cout << RESET << "\n\n";

    int nextLevel = u.getLevel() + 1;
    int xpToNextLevel = (nextLevel - 1) * (nextLevel - 1) * 10; 
    
    cout << "XP Progress (" << u.getXP() << " / " << xpToNextLevel << "):\n";
    renderProgressBar(u.getXP(), xpToNextLevel, YELLOW);
    
    cout << "\n" << CYAN << "--- Today's Active Quests ---\n" << RESET;
    
    if (habits.empty()) {
        cout << "No habits yet. Go back to the menu and add one!\n";
    } else {
        for (const Habit* h : habits) {
            h->display();
        }
    }

    cout << "\nPress Enter to return to the menu...";
    cin.get();
}

int main() {
    setCursorVisibility(false);
    clearScreen();
    
    typeText(string(CYAN) + "Initializing HABITQUEST System...\n" + RESET, 15);
    
    User player("Player1");
    vector<Habit*> activeHabits;
    Leaderboard lb;
    
    lb.addPlayer("IronCoder", 1450, 12);
    lb.addPlayer("DebugNinja", 820, 9);
    lb.addPlayer("SleepDeprived", 150, 3);
    
    showSpinner("Connecting to local .dat files", 600);
    if (Database::loadUserState(player)) {
        cout << GREEN << "User profile loaded successfully.\n" << RESET;
    }
    if (Database::loadHabits(activeHabits)) {
        cout << GREEN << "Habit list loaded successfully.\n" << RESET;
    }
    Sleep(800);

    bool isRunning = true;
    int habitCounter = activeHabits.size() + 1;

    vector<string> mainMenuOptions = {
        "View Dashboard (Stats & Health)",
        "Add a New Habit",
        "Log Habit Completion (Earn XP!)",
        "View Global Leaderboard (BST)",
        "Generate Visual Analytics (Python)",
        "Save & Exit Application"
    };

    while (isRunning) {
        // Use our new reusable menu engine!
        int choice = displayInteractiveMenu("HABITQUEST: MAIN MENU", mainMenuOptions) + 1;

        switch (choice) {
            case 1:
                showSpinner("Compiling Dashboard Stats", 400);
                displayDashboard(player, activeHabits);
                break;
                
            case 2: {
                // Reusing the menu for selecting a habit type
                vector<string> typeOptions = {
                    "Yes/No Task (e.g., Read a book)",
                    "Quantity Task (e.g., Drink 3000ml water)",
                    "Time-Based Task (e.g., Meditate for 10 mins)",
                    "Cancel / Go Back"
                };
                
                int type = displayInteractiveMenu("ADD A NEW HABIT: SELECT TYPE", typeOptions);
                
                if (type == 3) break; // User selected "Cancel"
                
                string name;
                cout << GREEN << "--- Add a New Habit ---\n" << RESET;
                cout << "Enter Habit Name: ";
                getline(cin, name);
                
                if (type == 0) { // Binary
                    activeHabits.push_back(new BinaryHabit(habitCounter++, name, 25));
                    cout << GREEN << "Habit added successfully!\n" << RESET;
                } else if (type == 1) { // Slider
                    int target;
                    string unit;
                    cout << "Target amount: ";
                    cin >> target;
                    cout << "Unit of measurement: ";
                    cin >> unit;
                    clearInputBuffer();
                    activeHabits.push_back(new SliderHabit(habitCounter++, name, 40, target, unit));
                    cout << GREEN << "Slider Habit added successfully!\n" << RESET;
                } else if (type == 2) { // Timer
                    int mins;
                    cout << "Target minutes: ";
                    cin >> mins;
                    clearInputBuffer();
                    activeHabits.push_back(new TimerHabit(habitCounter++, name, 50, mins));
                    cout << GREEN << "Timer Habit added successfully!\n" << RESET;
                }
                Sleep(1000);
                break;
            }
                
            case 3: {
                if(activeHabits.empty()) {
                    cout << MAGENTA << "--- Log Completion ---\n" << RESET;
                    cout << "No habits to complete. Add one first!\n";
                    Sleep(1500);
                    break;
                }
                
                // Dynamically build a menu out of the active habits!
                vector<string> habitOptions;
                for (const Habit* h : activeHabits) {
                    habitOptions.push_back(h->getName() + (h->getStatus() ? " (Done)" : ""));
                }
                habitOptions.push_back("Cancel / Go Back");

                int selectedHabitIndex = displayInteractiveMenu("LOG COMPLETION: SELECT A HABIT", habitOptions);
                
                if (selectedHabitIndex == activeHabits.size()) break; // Cancelled
                
                Habit* targetHabit = activeHabits[selectedHabitIndex];
                
                cout << MAGENTA << "--- Log Completion ---\n" << RESET;
                targetHabit->display();
                cout << "\n";
                
                if (targetHabit->logCompletion()) {
                    GamificationEngine::awardXP(player, targetHabit->getBaseXP());
                }
                
                cout << "\nPress Enter to return...";
                cin.get();
                break;
            }
            
            case 4:
                lb.addPlayer(player.getUsername(), player.getXP(), player.getLevel());
                lb.displayTopPlayers();
                cout << "Press Enter to return...";
                cin.get();
                break;

            case 5:
                cout << CYAN << "--- Analytics ---\n" << RESET;
                showSpinner("Exporting data and launching Python script", 1000);
                system("python analytics.py");
                cout << "\nPress Enter to return...";
                cin.get();
                break;

            case 6:
                showSpinner("Saving profile and serializing data", 1000);
                Database::saveUserState(player);
                Database::saveHabits(activeHabits);
                typeText(string(GREEN) + "Exiting HABITQUEST. Stay disciplined, " + player.getUsername() + "!\n" + RESET);
                isRunning = false;
                break;
        }
    }
    
    for(Habit* h : activeHabits) {
        delete h;
    }
    activeHabits.clear();

    return 0;
}