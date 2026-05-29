#include "solver.h"
#include <iostream>

Solver::Solver(Grid& grid) : grid(grid), callCount(0) {}

bool Solver::solve() {
    callCount = 0;
    return backtrack();
}

pair<int,int> Solver::findNextEmpty() {
    for (int r = 0; r < grid.size; r++)
        for (int c = 0; c < grid.size; c++)
            if (grid.board[r][c] == 0)
                return {r, c};
    return {-1, -1};   // no empty cell = puzzle solved
}

bool Solver::isCagePartiallyValid(int cageIdx, int row, int col, int val) {
    Cage& cage = grid.cages[cageIdx];

    // Collect all current values in this cage (including the val we want to place)
    vector<int> values;
    bool cageFull = true;

    for (auto& cell : cage.cells) {
        int v;
        if (cell.first == row && cell.second == col)
            v = val;   // the value we're trying
        else
            v = grid.board[cell.first][cell.second];

        if (v == 0) cageFull = false;
        values.push_back(v);
    }

    // If cage is fully filled, it must satisfy the constraint exactly
    if (cageFull)
        return cage.isSatisfied(values);

    // If cage is not fully filled yet, do partial checks
    // For ADD: partial sum must not already exceed target
    if (cage.op == Operation::ADD) {
    int partialSum = 0;
    for (int v : values)
        if (v != 0) partialSum += v;
    return partialSum <= cage.target;
}

    // For MULTIPLY: partial product must not exceed target
    if (cage.op == Operation::MULTIPLY) {
        int partialProduct = 1;
        for (int v : values)
            if (v != 0) partialProduct *= v;
        return partialProduct <= cage.target;
    }

    // For SUBTRACT and DIVIDE — only 2 cells, just allow until both filled
    // Full check happens when cageFull = true above
    return true;
}

bool Solver::backtrack() {
    callCount++;

    pair<int,int> cell = findNextEmpty();
int row = cell.first;
int col = cell.second;
    // Base case — no empty cell means puzzle is solved
    if (row == -1) return true;

    int cageIdx = grid.cageIndex[row][col];

    // Try every possible value 1 to N
    for (int val = 1; val <= grid.size; val++) {

        // Check 1: row and column uniqueness
        if (!grid.isRowColValid(row, col, val))
            continue;

        // Check 2: cage partial validity
        if (!isCagePartiallyValid(cageIdx, row, col, val))
            continue;

        // Place the value
        grid.board[row][col] = val;

        // Recurse — go to next empty cell
        if (backtrack())
            return true;

        // If recursion failed, undo (backtrack)
        grid.board[row][col] = 0;
    }

    // No value worked — signal failure to caller
    return false;
}