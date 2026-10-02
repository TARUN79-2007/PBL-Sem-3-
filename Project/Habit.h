#ifndef HABIT_H
#define HABIT_H

#include <string>

// --- ABSTRACT BASE CLASS ---
class Habit {
protected:
    int id;
    std::string name;
    bool isCompleted;
    int baseXP;

public:
    Habit(int _id, std::string _name, int _baseXP);
    virtual ~Habit() {} 

    // Getters
    int getId() const;
    std::string getName() const;
    bool getStatus() const;
    int getBaseXP() const;

    // Add this silent setter!
    void setStatus(bool status) { isCompleted = status; }

    // Pure virtual functions 
    virtual void display() const = 0; 
    virtual bool logCompletion() = 0; 
};

// --- DERIVED CLASS 1: Binary Habit (Yes / No) ---
class BinaryHabit : public Habit {
public:
    BinaryHabit(int _id, std::string _name, int _baseXP);
    
    void display() const override;
    bool logCompletion() override;
};

// --- DERIVED CLASS 2: Slider Habit (Quantity Based) ---
class SliderHabit : public Habit {
private:
    int currentAmount;
    int targetAmount;
    std::string unit; // e.g., "ml", "pages", "pushups"

public:
    SliderHabit(int _id, std::string _name, int _baseXP, int _targetAmount, std::string _unit);
    
    void display() const override;
    bool logCompletion() override;
};
// --- DERIVED CLASS 3: Time-Based Habit ---
class TimerHabit : public Habit {
private:
    int targetMinutes;

public:
    TimerHabit(int _id, std::string _name, int _baseXP, int _targetMinutes);
    
    void display() const override;
    bool logCompletion() override;
};

#endif