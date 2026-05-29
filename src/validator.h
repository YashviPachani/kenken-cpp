#pragma once
#include "grid.h"

class Validator {
public:
    // Check if the fully filled board is a valid solution
    static bool isValidSolution(const Grid& grid);

    // Check if any row has duplicate values
    static bool rowsValid(const Grid& grid);

    // Check if any column has duplicate values
    static bool colsValid(const Grid& grid);

    // Check if all cages are satisfied
    static bool cagesValid(const Grid& grid);
};