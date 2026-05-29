#pragma once
#include "cage.h"
#include <vector>

class Grid {
public:
    int size;                        // N (4 to 9)
    vector<vector<int>> board;       // N×N board, 0 = empty
    vector<Cage> cages;              // all cages in this puzzle
    vector<vector<int>> cageIndex;   // cageIndex[r][c] = which cage owns cell (r,c)

    // Constructor — creates empty N×N grid
    Grid(int size);

    // Add a cage to the grid
    void addCage(const Cage& cage);

    // Place a value on the board
    void setValue(int row, int col, int val);

    // Get value at a cell
    int getValue(int row, int col) const;

    // Display board in console (nicely formatted)
    void display() const;

    // Check if placing val at (row,col) is valid so far
    // (row unique + col unique)
    bool isRowColValid(int row, int col, int val) const;

    // Reset board to all zeros
    void reset();
};