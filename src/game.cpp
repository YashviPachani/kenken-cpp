#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;

Game::Game() : puzzle(nullptr), hintsUsed(0), moveCount(0), running(false) {}

Game::~Game() {
    if (puzzle) delete puzzle;
}

void Game::clearScreen() {
    cout << "\033[2J\033[1;1H";  // ANSI clear screen
}

string Game::getElapsedTime() {
    auto now = steady_clock::now();
    int secs = (int)duration_cast<seconds>(now - startTime).count();
    int m = secs / 60;
    int s = secs % 60;
    ostringstream oss;
    oss << setfill('0') << setw(2) << m << ":" << setw(2) << s;
    return oss.str();
}

void Game::showStatsBar() {
    cout << "\033[33m  Time: " << getElapsedTime()
         << "   Moves: " << moveCount
         << "   Hints: " << hintsUsed
         << "\033[0m\n";
    cout << "  " << string(45, '-') << "\n";
}

void Game::showMainMenu() {
    clearScreen();
    cout << "\n";
    cout << "  \033[32m+======================================+\033[0m\n";
    cout << "  \033[32m|         KENKEN PUZZLE GAME           |\033[0m\n";
    cout << "  \033[32m+======================================+\033[0m\n\n";
    cout << "  \033[33m[1]\033[0m  Play new puzzle\n";
    cout << "  \033[33m[2]\033[0m  Load puzzle from file\n";
    cout << "  \033[33m[3]\033[0m  Exit\n\n";
    cout << "  Enter choice: ";
}

void Game::startNewGame() {
    clearScreen();
    cout << "\n  \033[33mSelect grid size:\033[0m\n";
    cout << "  [1] 4x4\n  [2] 5x5\n  [3] 6x6\n";
    cout << "  Enter choice: ";

    int sizeChoice;
    cin >> sizeChoice;
    int gridSize = 4;
    if (sizeChoice == 2) gridSize = 5;
    else if (sizeChoice == 3) gridSize = 6;

    cout << "\n  \033[33mSelect difficulty:\033[0m\n";
    cout << "  [1] Easy   (more single-cell & + cages)\n";
    cout << "  [2] Medium (mixed operators)\n";
    cout << "  [3] Hard   (more * and / cages)\n";
    cout << "  Enter choice: ";

    int diff;
    cin >> diff;

    cout << "\n  \033[32mGenerating puzzle...\033[0m\n";

    if (puzzle) delete puzzle;
    puzzle = new Grid(gridSize);

    Generator gen(gridSize);
    *puzzle = gen.generate();

    // Reset game state
    while (!moveHistory.empty()) moveHistory.pop();
    hintsUsed = 0;
    moveCount = 0;
    startTime = steady_clock::now();
    running = true;

    playLoop();
}

bool Game::placeValue(int row, int col, int val) {
    if (row < 0 || row >= puzzle->size ||
        col < 0 || col >= puzzle->size) {
        cout << "  \033[31mInvalid row or col!\033[0m\n";
        return false;
    }

    if (val < 1 || val > puzzle->size) {
        cout << "  \033[31mValue must be between 1 and "
             << puzzle->size << "\033[0m\n";
        return false;
    }

    // Block overwriting a filled cell
    if (puzzle->board[row][col] != 0) {
        cout << "  \033[31mCell (" << row << "," << col
             << ") is already filled with "
             << puzzle->board[row][col]
             << "! Use [C] to clear it first.\033[0m\n";
        return false;
    }

    // Check row/col conflict
    if (!puzzle->isRowColValid(row, col, val)) {
        cout << "  \033[31mConflict! " << val
             << " already exists in that row or column.\033[0m\n";
        return false;
    }

    // Record move for undo
    Move m;
    m.row = row;
    m.col = col;
    m.prevValue = 0;
    moveHistory.push(m);

    puzzle->board[row][col] = val;
    moveCount++;
    return true;
}

void Game::clearCell(int row, int col) {
    if (row < 0 || row >= puzzle->size ||
        col < 0 || col >= puzzle->size) {
        cout << "  \033[31mInvalid row or col!\033[0m\n";
        return;
    }

    if (puzzle->board[row][col] == 0) {
        cout << "  \033[31mCell (" << row << "," << col
             << ") is already empty!\033[0m\n";
        return;
    }

    // Record for undo
    Move m;
    m.row = row;
    m.col = col;
    m.prevValue = puzzle->board[row][col];
    moveHistory.push(m);

    cout << "  \033[33mCleared (" << row << "," << col
         << ") — was " << puzzle->board[row][col] << "\033[0m\n";

    puzzle->board[row][col] = 0;
    moveCount++;
}

void Game::undoMove() {
    if (moveHistory.empty()) {
        cout << "  \033[31mNothing to undo!\033[0m\n";
        return;
    }
    Move m = moveHistory.top();
    moveHistory.pop();
    puzzle->board[m.row][m.col] = m.prevValue;
    cout << "  \033[33mUndo: cleared (" << m.row << "," << m.col << ")\033[0m\n";
}

void Game::giveHint() {
    // Solve a copy of the puzzle to get the solution
    Grid copy = *puzzle;
    Solver solver(copy);
    if (!solver.solve()) {
        cout << "  \033[31mCould not compute hint.\033[0m\n";
        return;
    }

    // Find a random empty cell and reveal it
    vector<pair<int,int>> emptyCells;
    for (int r = 0; r < puzzle->size; r++)
        for (int c = 0; c < puzzle->size; c++)
            if (puzzle->board[r][c] == 0)
                emptyCells.push_back({r, c});

    if (emptyCells.empty()) {
        cout << "  \033[33mBoard is already full!\033[0m\n";
        return;
    }

    // Pick first empty cell as hint
    int r = emptyCells[0].first;
    int c = emptyCells[0].second;
    int hintVal = copy.board[r][c];

    Move m;
    m.row = r; m.col = c;
    m.prevValue = 0;
    moveHistory.push(m);

    puzzle->board[r][c] = hintVal;
    hintsUsed++;
    moveCount++;

    cout << "  \033[32mHint: (" << r << "," << c
         << ") = " << hintVal << "\033[0m\n";
}

void Game::autoSolve() {
    cout << "  \033[33mAuto-solving...\033[0m\n";
    Solver solver(*puzzle);
    if (solver.solve()) {
        cout << "  \033[32mSolved!\033[0m\n";
    } else {
        cout << "  \033[31mNo solution found!\033[0m\n";
    }
}

void Game::savePuzzle(const string& filename) {
    ofstream f("../puzzles/" + filename);
    if (!f) {
        cout << "  \033[31mCould not save file!\033[0m\n";
        return;
    }

    f << puzzle->size << "\n";
    f << puzzle->cages.size() << "\n";

    for (auto& cage : puzzle->cages) {
        f << cage.target << " ";
        switch(cage.op) {
            case Operation::ADD:      f << "+"; break;
            case Operation::SUBTRACT: f << "-"; break;
            case Operation::MULTIPLY: f << "*"; break;
            case Operation::DIVIDE:   f << "/"; break;
            case Operation::NONE:     f << "N"; break;
        }
        f << " " << cage.cells.size();
        for (auto& cell : cage.cells)
            f << " " << cell.first << " " << cell.second;
        f << "\n";
    }

    // Save current board state
    for (int r = 0; r < puzzle->size; r++) {
        for (int c = 0; c < puzzle->size; c++)
            f << puzzle->board[r][c] << " ";
        f << "\n";
    }

    f.close();
    cout << "  \033[32mPuzzle saved to puzzles/" << filename << "\033[0m\n";
}

void Game::loadPuzzle(const string& filename) {
    ifstream f("../puzzles/" + filename);
    if (!f) {
        cout << "  \033[31mFile not found!\033[0m\n";
        return;
    }

    int sz;
    f >> sz;
    if (puzzle) delete puzzle;
    puzzle = new Grid(sz);

    int numCages;
    f >> numCages;

    for (int i = 0; i < numCages; i++) {
        int target, numCells;
        char opChar;
        f >> target >> opChar >> numCells;

        Operation op;
        switch(opChar) {
            case '+': op = Operation::ADD; break;
            case '-': op = Operation::SUBTRACT; break;
            case '*': op = Operation::MULTIPLY; break;
            case '/': op = Operation::DIVIDE; break;
            default:  op = Operation::NONE; break;
        }

        vector<Cell> cells;
        for (int j = 0; j < numCells; j++) {
            int r, c;
            f >> r >> c;
            cells.push_back({r, c});
        }
        puzzle->addCage(Cage(target, op, cells));
    }

    for (int r = 0; r < puzzle->size; r++)
        for (int c = 0; c < puzzle->size; c++)
            f >> puzzle->board[r][c];

    f.close();

    while (!moveHistory.empty()) moveHistory.pop();
    hintsUsed = 0;
    moveCount = 0;
    startTime = steady_clock::now();
    running = true;

    cout << "  \033[32mPuzzle loaded!\033[0m\n";
    playLoop();
}

 void Game::loadBestTimes() {
    ifstream f("../puzzles/besttimes.txt");
    if (!f) return;
    int size, secs;
    while (f >> size >> secs)
        bestTimes[size] = secs;
    f.close();
}

void Game::saveBestTimes() {
    ofstream f("../puzzles/besttimes.txt");
    for (auto& entry : bestTimes)
        f << entry.first << " " << entry.second << "\n";
    f.close();
}

void Game::showWinScreen() {
    clearScreen();

    auto now = steady_clock::now();
    int totalSecs = (int)duration_cast<seconds>(now - startTime).count();
    int gridSize  = puzzle->size;

    // Load existing best times
    loadBestTimes();

    bool beatRecord = false;
    bool firstTime  = false;

    if (bestTimes.find(gridSize) == bestTimes.end()) {
        // No record exists yet for this size
        firstTime = true;
        bestTimes[gridSize] = totalSecs;
        saveBestTimes();
    } else if (totalSecs < bestTimes[gridSize]) {
        // Beat the existing record
        beatRecord = true;
        bestTimes[gridSize] = totalSecs;
        saveBestTimes();
    }

    cout << "\n";
    cout << "  \033[32m+======================================+\033[0m\n";
    cout << "  \033[32m|         PUZZLE SOLVED!               |\033[0m\n";
    cout << "  \033[32m+======================================+\033[0m\n\n";

    puzzle->display();

    cout << "  \033[33mTime taken : " << getElapsedTime() << "\033[0m\n";
    cout << "  \033[33mMoves made : " << moveCount       << "\033[0m\n";
    cout << "  \033[33mHints used : " << hintsUsed       << "\033[0m\n\n";

    // Best time message
    if (firstTime) {
        cout << "  \033[32mFirst time solving " << gridSize << "x" << gridSize
             << "! Time recorded: " << getElapsedTime() << "\033[0m\n\n";
    } else if (beatRecord) {
        cout << "  \033[32m*** NEW BEST TIME for " << gridSize << "x" << gridSize
             << "! You beat the record! ***\033[0m\n\n";
    } else {
        // Show how far off the best time they were
        int best = bestTimes[gridSize];
        int diff = totalSecs - best;
        int bm = best / 60, bs = best % 60;
        ostringstream oss;
        oss << setfill('0') << setw(2) << bm << ":" << setw(2) << bs;
        cout << "  \033[33mBest time for " << gridSize << "x" << gridSize
             << ": " << oss.str() << "\033[0m\n";
        cout << "  \033[33mYou were " << diff
             << " seconds away from beating it!\033[0m\n\n";
    }
cout << "  Press Enter to return to menu...";
    cin.get();
    
}

void Game::playLoop() {
    while (running) {
        clearScreen();
        showStatsBar();
        puzzle->display();

        cout << "  \033[33mCommands:\033[0m\n";
        cout << "  [E] Enter value    [C] Clear cell\n";
        cout << "  [U] Undo           [H] Hint\n";
        cout << "  [A] Auto-solve     [S] Save progress\n";
        cout << "  [Q] Quit to menu\n";
        cout << "\n  > ";

        char cmd;
        cin >> cmd;
        cmd = toupper(cmd);

        if (cmd == 'E') {
            cout << "  Row Col Value: ";
            int r, c, v;
            if (!(cin >> r >> c >> v)) {
                cin.clear();                          // clear fail state
                cin.ignore(1000, '\n');               // flush bad input
                cout << "  \033[31mInvalid input! Enter 3 numbers: row col value\033[0m\n";
                cout << "  Press Enter to continue...";
                cin.get();
                continue;
            }
            if (placeValue(r, c, v)) {
    cout << "  \033[32mPlaced " << v
         << " at (" << r << "," << c << ")\033[0m\n";

  
}
            cout << "  Press Enter to continue...";
            cin.ignore(); cin.get();

         } else if (cmd == 'U') {
            undoMove();
            cout << "  Press Enter to continue...";
            cin.ignore(); cin.get();

        } else if (cmd == 'C') {
            cout << "  Row Col: ";
            int r, c;
            if (!(cin >> r >> c)) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "  \033[31mInvalid input! Enter 2 numbers: row col\033[0m\n";
                cout << "  Press Enter to continue...";
                cin.get();
                continue;
            }
            clearCell(r, c);
            cout << "  Press Enter to continue...";
            cin.ignore(); cin.get();

        } else if (cmd == 'H') {
            giveHint();
            if (Validator::isValidSolution(*puzzle)) {
                showWinScreen();
                running = false;
            }
            cout << "  Press Enter to continue...";
            cin.ignore(); cin.get();

        } else if (cmd == 'A') {
            autoSolve();
            puzzle->display();
            if (Validator::isValidSolution(*puzzle)) {
                showWinScreen();
                running = false;
            }
            cout << "  Press Enter to continue...";
            cin.ignore(); cin.get();

        }  else if (cmd == 'S') {
            cout << "  Enter filename to save progress: ";
            string fname;
            cin >> fname;
            savePuzzle(fname + ".txt");
            cout << "  Press Enter to continue...";
            cin.ignore(); cin.get();

        } else if (cmd == 'Q') {
            running = false;
        } else {
            cout << "  \033[31mInvalid command! Use E, C, U, H, A, S or Q\033[0m\n";
            cout << "  Press Enter to continue...";
            cin.ignore();
            cin.get();
        }
    }
}