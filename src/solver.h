#pragma once
#include "grid.h"

class Solver {
public:
    Grid& grid;
    int callCount;   // counts how many times solver was called (for stats)

    Solver(Grid& grid);

    // Main solve function — returns true if solved
    bool solve();

private:
    // Find the next empty cell, returns {-1,-1} if none (puzzle complete)
    pair<int,int> findNextEmpty();

    // Core recursive backtracking function
    bool backtrack();

    // Check if placing val at (row,col) is still valid for its cage
    // (partial check — cage may not be full yet)
    bool isCagePartiallyValid(int cageIdx, int row, int col, int val);
};