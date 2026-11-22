#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include <ctime>
#include <windows.h>
#include <conio.h>

#include "Utils.h"
#include "User.h"
#include "Habit.h"
#include "Database.h"
#include "GamificationEngine.h"
#include "Leaderboard.h"

using namespace std;

// --- TIME & UI HELPERS ---
string currentDate() {
    time_t t = time(nullptr);
    tm* p = localtime(&t);
    char b[16];
    strftime(b, sizeof(b), "%Y-%m-%d", p);
    return b;
}

void waitEnter() {
    cout << "\nPress Enter to return...";
    string s;
    getline(cin, s);
}

string readPassword() {
    string password;
    int ch;
    while ((ch = _getch()) != 13) {
        if (ch == 8) {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else if (ch >= 32 && ch <= 126) {
            password.push_back(static_cast<char>(ch));
            cout << '*';
        }
    }
    cout << '\n';
    return password;
}

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
struct AnalyticsData {
    string filepath;
    string username;
};

DWORD WINAPI LaunchAnalytics(LPVOID lpParam) {
    AnalyticsData* data = (AnalyticsData*)lpParam;
    string command = "python analytics.py \"" + data->filepath + "\" \"" + data->username + "\"";
    system(command.c_str());
    delete data; 
    return 0;
}

// --- REUSABLE INTERACTIVE MENU ENGINE ---
int displayInteractiveMenu(const string& title, const vector<string>& options) {
    int selectedIndex = 0;
    bool isChoosing = true;
    
    setCursorVisibility(false); 
    clearScreen();              

    while (isChoosing) {
        moveCursor(0, 0); 
        cout << CYAN << "====================================================\n" << RESET;
        cout << GREEN << "  " << title << "  \n" << RESET;
        cout << CYAN << "====================================================\n\n" << RESET;
        
        // Properly cast size() to int to fix -Wsign-compare warnings
        for (int i = 0; i < static_cast<int>(options.size()); ++i) {
            if (i == selectedIndex) {
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
                if (selectedIndex < 0) selectedIndex = static_cast<int>(options.size()) - 1;
            } else if (key == 80) { // DOWN
                selectedIndex++;
                if (selectedIndex >= static_cast<int>(options.size())) selectedIndex = 0;
            }
        } else if (key == 13) { // ENTER
            isChoosing = false;
        }
    }
    setCursorVisibility(true); 
    clearScreen();
    return selectedIndex;
}

bool readInt(const string& prompt, int& value) {
    cout << prompt;
    if (cin >> value && value > 0) {
        clearInputBuffer();
        return true;
    }
    cin.clear();
    clearInputBuffer();
    cout << RED << "Enter a positive whole number.\n" << RESET;
    return false;
}

void saveSession(User& user, vector<Habit*>& habits) {
    Database::saveUserState(user);
    Database::saveHabits(habits, user.getUsername());
    int done = 0, missed = 0;
    for (auto* h : habits) {
        (h->getStatus() ? done : missed)++;
    }
    Database::appendAnalytics(user, done, missed);
}

// --- APPLICATION MODULES ---
void dashboard(const User& u, const vector<Habit*>& hs) {
    cout << CYAN << "=============== HABITQUEST DASHBOARD ===============\n" << RESET;
    cout << "Player: " << GREEN << u.getUsername() << RESET << " | Level: " << YELLOW << u.getLevel() << RESET << " | XP: " << u.getXP() << "\n";
    cout << "Streak: " << MAGENTA << u.getStreak() << " days" << RESET << " | Hearts: " << RED;
    for (int i = 0; i < 5; ++i) cout << (i < u.getHearts() ? "<3 " : "- ");
    cout << RESET << "\n\nToday's active quests:\n";
    
    if (hs.empty()) cout << "No habits yet. Add your first habit!\n";
    for (const auto* h : hs) h->display();
    
    int threshold = u.getLevel() * u.getLevel() * 10;
    cout << "\nXP Progress (" << u.getXP() << " / " << threshold << "):\n";
    renderProgressBar(u.getXP(), threshold, YELLOW);
}

void addHabit(vector<Habit*>& hs, int& nextId) {
    int type = displayInteractiveMenu("ADD A NEW HABIT", {"Yes/No Task", "Quantity Task", "Time-Based Task", "Cancel"});
    if (type == 3) return;
    
    cout << GREEN << "--- Add a New Habit ---\n" << RESET;
    string name;
    cout << "Habit name: ";
    getline(cin, name);
    if (name.empty()) { cout << RED << "Name cannot be empty.\n" << RESET; return; }
    
    if (type == 0) {
        hs.push_back(new BinaryHabit(nextId++, name, 25));
    } else if (type == 1) {
        int target;
        if (!readInt("Target amount: ", target)) return;
        string unit;
        cout << "Unit (e.g., glasses, pages): ";
        getline(cin, unit);
        if (unit.empty()) unit = "units";
        hs.push_back(new SliderHabit(nextId++, name, 40, target, unit));
    } else {
        int mins;
        if (!readInt("Target minutes: ", mins)) return;
        hs.push_back(new TimerHabit(nextId++, name, 50, mins));
    }
    cout << GREEN << "Habit added successfully!\n" << RESET;
}

void logHabit(User& u, vector<Habit*>& hs) {
    if (hs.empty()) { cout << "No habits to complete.\n"; return; }
    
    vector<string> opts;
    for (auto* h : hs) opts.push_back(h->getName() + (h->getStatus() ? " [Done]" : ""));
    opts.push_back("Cancel / Go Back");
    
    int i = displayInteractiveMenu("LOG HABIT COMPLETION", opts);
    if (i >= static_cast<int>(hs.size())) return;
    
    cout << MAGENTA << "--- Log Completion ---\n" << RESET;
    hs[i]->display();
    cout << "\n";
    if (hs[i]->logCompletion()) {
        GamificationEngine::awardXP(u, hs[i]->getBaseXP());
    }
}

void deleteHabit(vector<Habit*>& hs) {
    if (hs.empty()) { cout << "No habits to delete.\n"; return; }
    
    vector<string> opts;
    for (auto* h : hs) opts.push_back("DELETE: " + h->getName());
    opts.push_back("Cancel / Go Back");
    
    int i = displayInteractiveMenu("DELETE A HABIT", opts);
    if (i >= static_cast<int>(hs.size())) return;
    
    delete hs[i];
    hs.erase(hs.begin() + i);
    cout << RED << "Habit deleted.\n" << RESET;
}

void runPlayer(User& u) {
    vector<Habit*> habits;
    Database::loadHabits(habits, u.getUsername());
    
    string dateFile = Database::userDirectory(u.getUsername()) + "/last_login.dat", last;
    ifstream d(dateFile);
    d >> last;
    d.close();
    
    string today = currentDate();
    if (!last.empty() && last != today) {
        for (Habit* h : habits) h->resetDaily();
        cout << YELLOW << "New day detected. Daily habit progress has been reset.\n" << RESET;
        Sleep(1500);
    }
    ofstream out(dateFile);
    out << today;
    out.close();
    
    int nextId = 1;
    for (auto* h : habits) {
        if (h->getId() >= nextId) nextId = h->getId() + 1;
    }
    
    bool active = true;
    while (active) {
        int c = displayInteractiveMenu("HABITQUEST | " + u.getUsername(), {
            "View Dashboard (Stats & Health)",
            "Add a New Habit",
            "Log Habit Completion (Earn XP!)",
            "Delete a Habit",
            "Global Leaderboard (Multiplayer)",
            "Generate Visual Analytics (Python)",
            "Save & Logout"
        });
        
        switch (c) {
            case 0: dashboard(u, habits); waitEnter(); break;
            case 1: addHabit(habits, nextId); waitEnter(); break;
            case 2: logHabit(u, habits); waitEnter(); break;
            case 3: deleteHabit(habits); waitEnter(); break;
            case 4: {
                Database::saveUserState(u);
                Leaderboard board;
                board.display(Database::loadAllUsers());
                waitEnter();
                break;
            }
            case 5: {
                cout << CYAN << "--- Visual Analytics ---\n" << RESET;
                showSpinner("Exporting data and launching Python engine", 500);
                
                AnalyticsData* aData = new AnalyticsData{
                    Database::userDirectory(u.getUsername()) + "/tracker_data.csv", 
                    u.getUsername()
                };
                
                CreateThread(NULL, 0, LaunchAnalytics, aData, 0, NULL);
                cout << GREEN << "\nVisual Dashboard launched concurrently!\n" << RESET;
                waitEnter();
                break;
            }
            case 6: 
                saveSession(u, habits);
                cout << GREEN << "Progress saved. Logged out: " << u.getUsername() << "\n" << RESET;
                active = false; 
                Sleep(1000);
                break;
        }
    }
    for (Habit* h : habits) delete h;
}

int main() {
    setCursorVisibility(false);
    string status;
    
    typeText(string(CYAN) + "Initializing HABITQUEST System...\n" + RESET, 15);
    showSpinner("Connecting to local databases", 400);
    
    if (!Database::initialize(status)) {
        cerr << RED << status << RESET << '\n';
        return 1;
    }
    
    bool running = true;
    while (running) {
        int c = displayInteractiveMenu("WELCOME TO HABITQUEST", {
            "Login",
            "Register New Account",
            "Set Password for Migrated 'Player' Account",
            "Exit Game"
        });
        
        if (c == 0) {
            string name, pw, msg;
            cout << GREEN << "--- LOGIN ---\n" << RESET;
            cout << "Username: ";
            getline(cin, name);
            cout << "Password: ";
            pw = readPassword();
            if (Database::authenticate(name, pw, msg)) {
                User u(name);
                Database::loadUserState(u);
                cout << GREEN << msg << RESET << '\n';
                Sleep(800);
                runPlayer(u);
            } else {
                cout << RED << msg << RESET << '\n';
                waitEnter();
            }
        } else if (c == 1) {
            string name, pw, msg;
            cout << GREEN << "--- REGISTER ---\n" << RESET;
            cout << "Choose username (3-20 letters/numbers/_/-): ";
            getline(cin, name);
            cout << "Choose password (minimum 8 characters): ";
            pw = readPassword();
            if (Database::registerAccount(name, pw, msg)) cout << GREEN << msg << RESET << '\n';
            else cout << RED << msg << RESET << '\n';
            waitEnter();
        } else if (c == 2) {
            string pw, msg;
            cout << GREEN << "--- SECURE LEGACY ACCOUNT ---\n" << RESET;
            cout << "New password for legacy Player account (min 8 chars): ";
            pw = readPassword();
            if (Database::resetLegacyPassword("Player", pw, msg)) cout << GREEN << msg << RESET << '\n';
            else cout << RED << msg << RESET << '\n';
            waitEnter();
        } else {
            running = false;
        }
    }
    cout << CYAN << "HabitQuest closed. Stay consistent!\n" << RESET;
    return 0;
}