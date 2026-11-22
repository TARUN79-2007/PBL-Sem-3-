#ifndef HABIT_H
#define HABIT_H

#include <string>

class Habit {
protected:
    int id;
    std::string name;
    bool isCompleted;
    int baseXP;

public:
    Habit(int i, std::string n, int xp);
    virtual ~Habit() {}

    int getId() const;
    std::string getName() const;
    bool getStatus() const;
    int getBaseXP() const;
    void setStatus(bool s) { isCompleted = s; }

    virtual void display() const = 0;
    virtual bool logCompletion() = 0;
    virtual void resetDaily() { isCompleted = false; }
};

class BinaryHabit : public Habit {
public:
    BinaryHabit(int i, std::string n, int xp);
    void display() const override;
    bool logCompletion() override;
};

class SliderHabit : public Habit {
private:
    int currentAmount;
    int targetAmount;
    std::string unit;

public:
    SliderHabit(int i, std::string n, int xp, int target, std::string u);
    int getCurrent() const { return currentAmount; }
    int getTarget() const { return targetAmount; }
    std::string getUnit() const { return unit; }
    void setCurrent(int n) { currentAmount = n; }

    void display() const override;
    bool logCompletion() override;
    void resetDaily() override { isCompleted = false; currentAmount = 0; }
};

class TimerHabit : public Habit {
private:
    int targetMinutes;

public:
    TimerHabit(int i, std::string n, int xp, int mins);
    int getTargetMinutes() const { return targetMinutes; }
    void display() const override;
    bool logCompletion() override;
};

#endif