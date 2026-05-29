#pragma once
#include "grid.h"
#include <random>

class Generator {
public:
    int size;
    mt19937 rng;   // random number generator

    Generator(int size);

    // Full pipeline — returns a Grid ready to be played
    Grid generate();

private:
    // Step 1: fill board with a valid latin square
    void generateLatinSquare(Grid& grid);

    // Step 2: partition all cells into cages
    void generateCages(Grid& grid);

    // Step 3: for a given cage with known values, decide op + target
    Cage buildCage(vector<Cell>& cells, vector<vector<int>>& board);

    // Helper: shuffle a vector randomly
    template<typename T>
    void shuffleVec(vector<T>& v);
};