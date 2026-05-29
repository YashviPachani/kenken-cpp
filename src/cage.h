#pragma once
#include <vector>
#include <utility>   // for pair

using namespace std;

// Each cell is represented as (row, col)
using Cell = pair<int, int>;

// Supported operations
enum class Operation {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    NONE   // for single-cell cages
};

class Cage {
public:
    int target;              // the number the cage must produce
    Operation op;            // which operation
    vector<Cell> cells;      // which cells belong to this cage

    // Constructor
    Cage(int target, Operation op, vector<Cell> cells);

    // Check if the current values in this cage satisfy the constraint
    // values = the numbers currently placed in this cage's cells
    bool isSatisfied(vector<int>& values) const;

    // Display cage info
    void display() const;
};