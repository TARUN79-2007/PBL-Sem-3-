#include "Utils.h"
#include <iostream>
#include <limits>
#include <windows.h> // Replaced thread/chrono with native Windows header

using namespace std;

void clearScreen() {
    cout << "\033[2J\033[1;1H";
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void typeText(const string &text, int delayMs) {
    for (char c : text) {
        cout << c << flush;
        Sleep(delayMs); // Native Windows sleep
    }
}

void showSpinner(const string &message, int durationMs) {
    const char chars[] = {'|', '/', '-', '\\'};
    int steps = durationMs / 100;

    for (int i = 0; i < steps; ++i) {
        cout << "\r" << CYAN << message << " " << chars[i % 4] << RESET << flush;
        Sleep(100);
    }
    cout << "\r" << GREEN << message << " Complete!   " << RESET << "\n";
    Sleep(300);
}

void showLoadingBar(const string &task, int durationMs) {
    int steps = 20;
    int stepTime = durationMs / steps;

    cout << task << "\n[";
    for (int i = 0; i < steps; ++i) {
        cout << GREEN << "=" << RESET << flush;
        Sleep(stepTime);
    }
    cout << "] Done!\n";
    Sleep(400);
}

void renderProgressBar(int current, int total, const string &color) {
    if (total <= 0) return;

    int barWidth = 30;
    float progress = static_cast<float>(current) / total;
    if (progress > 1.0f) progress = 1.0f;

    int pos = static_cast<int>(barWidth * progress);

    cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << color << "=" << RESET;
        else if (i == pos) cout << color << ">" << RESET;
        else cout << " ";
    }
    cout << "] " << int(progress * 100.0) << "%\n";
}