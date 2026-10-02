#ifndef UTILS_H
#define UTILS_H

#include <string>

// ANSI Escape Codes for CLI Gamification UI
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

// UI Helper Functions
void clearScreen();
void clearInputBuffer();
void typeText(const std::string& text, int delayMs = 15);
void showSpinner(const std::string& message, int durationMs = 1200);
void showLoadingBar(const std::string& task, int durationMs = 1000);
void renderProgressBar(int current, int total, const std::string& color = GREEN);

#endif