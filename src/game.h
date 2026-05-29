#pragma once
#include "grid.h"
#include "solver.h"
#include "validator.h"
#include "generator.h"
#include <stack>
#include <chrono>
#include <string>
#include <map>

using namespace std;
using namespace chrono;

// Stores one move so we can undo it
struct Move {
    int row, col, prevValue;
};

class Game {
public:
    Grid* puzzle;
    stack<Move> moveHistory;     // for undo
    int hintsUsed;
    int moveCount;
    time_point<steady_clock> startTime;
    bool running;

    Game();
    ~Game();

    // Main menu
    void showMainMenu();

    // Start a new game — picks size and difficulty
    void startNewGame();

    // The actual game loop
    void playLoop();

    // Place a value — validates and records move
    bool placeValue(int row, int col, int val);
    
    void clearCell(int row, int col);

    // Undo last move
    void undoMove();

    // Give a hint — reveals one correct cell
    void giveHint();

    // Auto solve the puzzle
    void autoSolve();

    // Save puzzle to file
    void savePuzzle(const string& filename);

    // Load puzzle from file
    void loadPuzzle(const string& filename);

    // Show elapsed time
    string getElapsedTime();

    // Show stats bar
    void showStatsBar();

    // Clear console
    void clearScreen();

    // Show end screen when puzzle is solved
    void showWinScreen();

 void loadBestTimes();
void saveBestTimes();
map<int, int> bestTimes;
};