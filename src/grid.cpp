#include "grid.h"
#include <iostream>
#include <iomanip>   // for setw (formatted output)

Grid::Grid(int size) : size(size) {
    // Initialize board with all zeros
    board.assign(size, vector<int>(size, 0));
    // Initialize cageIndex with -1 (no cage assigned yet)
    cageIndex.assign(size, vector<int>(size, -1));
}

void Grid::addCage(const Cage& cage) {
    int idx = cages.size();   // index of this new cage
    cages.push_back(cage);
    // mark each cell of this cage in cageIndex
    for (auto& cell : cage.cells)
        cageIndex[cell.first][cell.second] = idx;
}

void Grid::setValue(int row, int col, int val) {
    board[row][col] = val;
}

int Grid::getValue(int row, int col) const {
    return board[row][col];
}

bool Grid::isRowColValid(int row, int col, int val) const {
    // Check row — no duplicate
    for (int c = 0; c < size; c++)
        if (c != col && board[row][c] == val) return false;

    // Check column — no duplicate
    for (int r = 0; r < size; r++)
        if (r != row && board[r][col] == val) return false;

    return true;
}

void Grid::reset() {
    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            board[r][c] = 0;
}

void Grid::display() const {
    
    // ── Cage ID grid ──
    // Each cell shows its cage number instead of borders
    cout << "\n";

    // Column headers
    cout << "      ";
    for (int c = 0; c < size; c++)
        cout << " C" << c << " ";
    cout << "\n";

    // Top border
    cout << "      ";
    for (int c = 0; c < size; c++) cout << "----";
    cout << "-\n";

    for (int r = 0; r < size; r++) {
        // Row with cage numbers (left side)
        cout << "  R" << r << " |";
        for (int c = 0; c < size; c++) {
            int idx = cageIndex[r][c];
            if (idx >= 0)
                cout << "\033[33m " << setw(2) << left << (idx + 1) << "\033[0m|";
            else
                cout << "  ?|";
        }

        // Row with values (right side) — shown next to cage grid
        cout << "        |";
        for (int c = 0; c < size; c++) {
            if (board[r][c] == 0)
                cout << " . |";
            else
                cout << "\033[36m " << board[r][c] << " \033[0m|";
        }
        cout << "\n";

        // Row separator
        cout << "      ";
        for (int c = 0; c < size; c++) cout << "----";
        cout << "-";
        cout << "        ";
        for (int c = 0; c < size; c++) cout << "----";
        cout << "-\n";
    }

    // ── Cage explanation ──
    cout << "\n  \033[33mCage Information:\033[0m\n";
    cout << "  " << string(45, '-') << "\n";

    for (int i = 0; i < (int)cages.size(); i++) {
        string opStr, opWord;
        switch (cages[i].op) {
            case Operation::ADD:
                opStr = "+"; opWord = "sum must be"; break;
            case Operation::SUBTRACT:
                opStr = "-"; opWord = "difference must be"; break;
            case Operation::MULTIPLY:
                opStr = "*"; opWord = "product must be"; break;
            case Operation::DIVIDE:
                opStr = "/"; opWord = "quotient must be"; break;
            case Operation::NONE:
                opStr = " "; opWord = "value must be"; break;
        }

        // Cage number and target
        cout << "  \033[33mCage " << (i + 1) << "\033[0m → "
             << "target \033[36m" << cages[i].target << opStr << "\033[0m"
             << "  |  cells: ";

        // List cells
        for (int j = 0; j < (int)cages[i].cells.size(); j++) {
            cout << "(" << cages[i].cells[j].first
                 << "," << cages[i].cells[j].second << ")";
            if (j < (int)cages[i].cells.size() - 1) cout << ", ";
        }

        cout << "  |  " << opWord << " " << cages[i].target << "\n";
    }

    cout << "\n  \033[90mHow to read: Left grid = cage numbers, Right grid = your answers\033[0m\n";
    cout << "  \033[90mCells with the same number belong to the same cage.\033[0m\n";
    cout << "  \033[90mFill the right grid using row/col — digits 1 to "
         << size << " in every row and col.\033[0m\n\n";

}