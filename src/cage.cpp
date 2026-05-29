#include "cage.h"
#include <iostream>
#include <algorithm>   // for sort, used in divide/subtract check

Cage::Cage(int target, Operation op, vector<Cell> cells)
    : target(target), op(op), cells(cells) {}

bool Cage::isSatisfied(vector<int>& values) const {
    // values must be fully filled (no zeros)
    for (int v : values)
        if (v == 0) return false;

    switch (op) {
        case Operation::NONE: {
            // single cell — value must equal target directly
            return values[0] == target;
        }

        case Operation::ADD: {
            int sum = 0;
            for (int v : values) sum += v;
            return sum == target;
        }

        case Operation::MULTIPLY: {
            int product = 1;
            for (int v : values) product *= v;
            return product == target;
        }

        case Operation::SUBTRACT: {
            // only 2 cells allowed for subtract
            // either order: |a - b| == target
            return abs(values[0] - values[1]) == target;
        }

        case Operation::DIVIDE: {
            int a = values[0], b = values[1];

            bool option1 = (b != 0 && a % b == 0 && a / b == target);
            bool option2 = (a != 0 && b % a == 0 && b / a == target);

            return option1 || option2;
        }
    }
    return false;
}

void Cage::display() const {
    string opStr;
    switch (op) {
        case Operation::ADD:      opStr = "+"; break;
        case Operation::SUBTRACT: opStr = "-"; break;
        case Operation::MULTIPLY: opStr = "*"; break;
        case Operation::DIVIDE:   opStr = "/"; break;
        case Operation::NONE:     opStr = " "; break;
    }
    cout << "Cage [" << target << opStr << "] cells: ";
    for (auto& c : cells)
        cout << "(" << c.first << "," << c.second << ") ";
    cout << endl;
}