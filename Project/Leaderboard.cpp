#include "Leaderboard.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>

using namespace std;

Leaderboard::Leaderboard() : root(nullptr) {}

// Destructor cleans up dynamic memory when the program closes
Leaderboard::~Leaderboard() {
    destroyTree(root);
}

void Leaderboard::destroyTree(BSTNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

void Leaderboard::addPlayer(string name, int xp, int level) {
    root = insertRec(root, name, xp, level);
}

// O(log n) Insertion based on XP
BSTNode* Leaderboard::insertRec(BSTNode* node, string name, int xp, int level) {
    // Found an empty spot, insert the new node
    if (node == nullptr) {
        return new BSTNode(name, xp, level);
    }

    // If XP is less, go left. If greater or equal, go right.
    if (xp < node->xp) {
        node->left = insertRec(node->left, name, xp, level);
    } else {
        node->right = insertRec(node->right, name, xp, level);
    }
    return node;
}

void Leaderboard::displayTopPlayers() const {
    cout << CYAN << "=================================================\n" << RESET;
    cout << YELLOW << "         GLOBAL LEADERBOARD (BST RANKING)\n" << RESET;
    cout << CYAN << "=================================================\n" << RESET;
    
    // Formatting the table headers
    cout << left << setw(10) << "Rank" << setw(15) << "Player" << setw(10) << "Level" << "XP\n";
    cout << "-------------------------------------------------\n";
    
    if (root == nullptr) {
        cout << "No data available.\n";
        return;
    }

    int rank = 1;
    inOrderRec(root, rank); // Start the traversal
    cout << "\n";
}

// ALGORITHM: Reverse In-Order Traversal (Right -> Root -> Left)
// This guarantees we print the highest XP first!
void Leaderboard::inOrderRec(BSTNode* node, int& rank) const {
    if (node == nullptr || rank > 10) return; // Only show top 10

    inOrderRec(node->right, rank); // 1. Traverse Right (Highest XP)

    if (rank <= 10) {
        // 2. Process Root (Print the current player)
        // Add special colors for the podium finishers
        if (rank == 1) cout << YELLOW;      // Gold for 1st
        else if (rank == 2) cout << CYAN;   // Diamond/Cyan for 2nd
        else if (rank == 3) cout << MAGENTA;// Magenta for 3rd
        else cout << RESET;

        cout << left << setw(10) << ("#" + to_string(rank)) 
             << setw(15) << node->username 
             << setw(10) << node->level 
             << node->xp << " XP\n" << RESET;
        rank++;
    }

    inOrderRec(node->left, rank); // 3. Traverse Left (Lower XP)
}