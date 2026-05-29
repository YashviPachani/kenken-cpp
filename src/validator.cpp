#include "validator.h"
#include <set>

bool Validator::rowsValid(const Grid& grid) {
    for (int r = 0; r < grid.size; r++) {
        set<int> seen;
        for (int c = 0; c < grid.size; c++) {
            int v = grid.board[r][c];
            if (v == 0 || seen.count(v)) return false;
            seen.insert(v);
        }
    }
    return true;
}

bool Validator::colsValid(const Grid& grid) {
    for (int c = 0; c < grid.size; c++) {
        set<int> seen;
        for (int r = 0; r < grid.size; r++) {
            int v = grid.board[r][c];
            if (v == 0 || seen.count(v)) return false;
            seen.insert(v);
        }
    }
    return true;
}

bool Validator::cagesValid(const Grid& grid) {
    for (const Cage& cage : grid.cages) {
        vector<int> values;
        for (auto& cell : cage.cells)
            values.push_back(grid.board[cell.first][cell.second]);
        if (!cage.isSatisfied(values)) return false;
    }
    return true;
}

bool Validator::isValidSolution(const Grid& grid) {
    return rowsValid(grid) && colsValid(grid) && cagesValid(grid);
}