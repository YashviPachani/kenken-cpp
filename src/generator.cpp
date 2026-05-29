#include "generator.h"
#include "solver.h"
#include <algorithm>
#include <iostream>
#include <chrono>

Generator::Generator(int size) : size(size) {
    rng.seed((unsigned int)chrono::steady_clock::now().time_since_epoch().count());
}

template<typename T>
void Generator::shuffleVec(vector<T>& v) {
    shuffle(v.begin(), v.end(), rng);
}

void Generator::generateLatinSquare(Grid& grid) {
    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            grid.board[r][c] = (r + c) % size + 1;

    // Shuffle rows
    vector<int> rowOrder(size);
    iota(rowOrder.begin(), rowOrder.end(), 0);
    shuffleVec(rowOrder);

    vector<vector<int>> newBoard(size, vector<int>(size));
    for (int r = 0; r < size; r++)
        newBoard[r] = grid.board[rowOrder[r]];
    grid.board = newBoard;

    // Shuffle columns
    vector<int> colOrder(size);
    iota(colOrder.begin(), colOrder.end(), 0);
    shuffleVec(colOrder);

    for (int r = 0; r < size; r++) {
        vector<int> newRow(size);
        for (int c = 0; c < size; c++)
            newRow[c] = grid.board[r][colOrder[c]];
        grid.board[r] = newRow;
    }

    // Shuffle values
    vector<int> vals(size);
    iota(vals.begin(), vals.end(), 1);
    shuffleVec(vals);

    vector<int> valueMap(size + 1);
    for (int i = 0; i < size; i++)
        valueMap[i + 1] = vals[i];

    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            grid.board[r][c] = valueMap[grid.board[r][c]];
}

void Generator::generateCages(Grid& grid) {
    vector<vector<bool>> assigned(size, vector<bool>(size, false));

    vector<Cell> allCells;
    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            allCells.push_back(Cell(r, c));
    shuffleVec(allCells);

    vector<pair<int,int>> dirs;
    dirs.push_back({-1, 0});
    dirs.push_back({ 1, 0});
    dirs.push_back({ 0,-1});
    dirs.push_back({ 0, 1});

    for (int ci = 0; ci < (int)allCells.size(); ci++) {
        int sr = allCells[ci].first;
        int sc = allCells[ci].second;

        if (assigned[sr][sc]) continue;

        vector<Cell> cageCells;
        cageCells.push_back(Cell(sr, sc));
        assigned[sr][sc] = true;

        // Find unassigned neighbors of starting cell
        vector<Cell> neighbors;
        for (int d = 0; d < (int)dirs.size(); d++) {
            int nr = sr + dirs[d].first;
            int nc = sc + dirs[d].second;
            if (nr >= 0 && nr < size && nc >= 0 && nc < size && !assigned[nr][nc])
                neighbors.push_back(Cell(nr, nc));
        }
        shuffleVec(neighbors);

        if (!neighbors.empty()) {
            int nr = neighbors[0].first;
            int nc = neighbors[0].second;
            cageCells.push_back(Cell(nr, nc));
            assigned[nr][nc] = true;

            // 30% chance to add a 3rd cell
            uniform_int_distribution<int> coin(1, 100);
            if (coin(rng) <= 30) {
                vector<Cell> neighbors2;
                for (int d = 0; d < (int)dirs.size(); d++) {
                    int r2 = nr + dirs[d].first;
                    int c2 = nc + dirs[d].second;
                    if (r2 >= 0 && r2 < size && c2 >= 0 && c2 < size && !assigned[r2][c2])
                        neighbors2.push_back(Cell(r2, c2));
                }
                shuffleVec(neighbors2);
                if (!neighbors2.empty()) {
                    int r2 = neighbors2[0].first;
                    int c2 = neighbors2[0].second;
                    cageCells.push_back(Cell(r2, c2));
                    assigned[r2][c2] = true;
                }
            }
        }

        Cage cage = buildCage(cageCells, grid.board);
        grid.addCage(cage);
    }
}

Cage Generator::buildCage(vector<Cell>& cells, vector<vector<int>>& board) {
    vector<int> vals;
    for (int i = 0; i < (int)cells.size(); i++) {
        int r = cells[i].first;
        int c = cells[i].second;
        vals.push_back(board[r][c]);
    }

    // Single cell
    if (cells.size() == 1)
        return Cage(vals[0], Operation::NONE, cells);

    // 3+ cells — only ADD or MULTIPLY
    if (cells.size() >= 3) {
        uniform_int_distribution<int> coin(0, 1);
        if (coin(rng) == 0) {
            int sum = 0;
            for (int i = 0; i < (int)vals.size(); i++) sum += vals[i];
            return Cage(sum, Operation::ADD, cells);
        } else {
            int product = 1;
            for (int i = 0; i < (int)vals.size(); i++) product *= vals[i];
            return Cage(product, Operation::MULTIPLY, cells);
        }
    }

    // 2-cell cage — try all operators
    int a = vals[0], b = vals[1];

    vector<pair<Operation,int>> validOps;
    validOps.push_back({Operation::ADD,      a + b});
    validOps.push_back({Operation::SUBTRACT, abs(a - b)});
    validOps.push_back({Operation::MULTIPLY, a * b});

    if (a % b == 0) validOps.push_back({Operation::DIVIDE, a / b});
    else if (b % a == 0) validOps.push_back({Operation::DIVIDE, b / a});

    uniform_int_distribution<int> pick(0, (int)validOps.size() - 1);
    int chosen = pick(rng);
    Operation op = validOps[chosen].first;
    int target   = validOps[chosen].second;

    return Cage(target, op, cells);
}

Grid Generator::generate() {
    Grid grid(size);
    generateLatinSquare(grid);
    generateCages(grid);
    grid.reset();
    return grid;
} 