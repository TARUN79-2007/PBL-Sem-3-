#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>

// A node in our Binary Search Tree
struct BSTNode {
    std::string username;
    int xp;
    int level;
    BSTNode* left;
    BSTNode* right;

    BSTNode(std::string name, int _xp, int _level) 
        : username(name), xp(_xp), level(_level), left(nullptr), right(nullptr) {}
};

class Leaderboard {
private:
    BSTNode* root;

    // Recursive helper algorithms
    BSTNode* insertRec(BSTNode* node, std::string name, int xp, int level);
    void inOrderRec(BSTNode* node, int& rank) const;
    void destroyTree(BSTNode* node); // Prevents memory leaks!

public:
    Leaderboard();
    ~Leaderboard();

    void addPlayer(std::string name, int xp, int level);
    void displayTopPlayers() const;
};

#endif