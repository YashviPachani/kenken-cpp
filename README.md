# KenKen Puzzle Game 🧩

A fully interactive, console-based **KenKen puzzle game** built in **C++** from scratch.
Features a random puzzle generator, a backtracking solver with constraint propagation,
live gameplay with timer, hints, undo, save/load, and best time tracking across sessions.

---

## Table of Contents
- [What is KenKen?](#what-is-kenken)
- [Game Rules](#game-rules)
- [Features](#features)
- [Project Structure](#project-structure)
- [How to Compile](#how-to-compile)
- [How to Play](#how-to-play)
- [Commands](#commands)
- [Sample Gameplay](#sample-gameplay)
- [Technical Concepts](#technical-concepts)

---

## What is KenKen?

KenKen is a mathematical puzzle invented in Japan in 2004. It combines the logic
of Sudoku with basic arithmetic. The name translates to **"cleverness squared"**.

The goal is to fill an N×N grid with digits so that:
- Every **row** contains each digit from 1 to N exactly once
- Every **column** contains each digit from 1 to N exactly once
- Grouped cells called **cages** satisfy a given arithmetic target

---

## Game Rules

### The Grid
- The puzzle is played on an **N×N grid** (this game supports 4×4, 5×5, and 6×6)
- Digits used are **1 through N** - for a 4×4 grid, digits are 1, 2, 3, 4

### Rows and Columns
- Every **row** must contain each digit from 1 to N **exactly once** - no repeats
- Every **column** must contain each digit from 1 to N **exactly once** - no repeats
- This rule is identical to Sudoku's row/column rule

### Cages
- The grid is divided into outlined groups of cells called **cages**
- Each cage has a **target number** and an **arithmetic operator** (+, −, ×, ÷)
- The numbers placed in a cage's cells must combine using the operator to equal the target

### Operators Explained

| Operator | Rule | Example |
|----------|------|---------|
| `+` Addition | All values in the cage must **sum** to the target | Cage `6+` with cells (1,2,3) → 1+2+3 = 6 ✔ |
| `−` Subtraction | The **difference** of the two values must equal the target | Cage `1-` with cells (3,2) → 3−2 = 1 ✔ |
| `×` Multiplication | All values must **multiply** to the target | Cage `6×` with cells (2,3) → 2×3 = 6 ✔ |
| `÷` Division | The **quotient** of the two values must equal the target | Cage `2÷` with cells (4,2) → 4÷2 = 2 ✔ |
| None | A **single-cell** cage — the digit is fixed directly | Cage `3` → cell must contain 3 |

### Important Notes
- Subtraction and division cages always have **exactly 2 cells**
- For subtraction/division, **order does not matter** - only the result counts
- Addition and multiplication cages can have **2 or more cells**
- A digit **can repeat inside a cage** as long as rows and columns stay valid
  - Example: cage `4+` in a 4×4 grid can be filled as (2,2) since 2+2=4,
    as long as those two cells are in different rows and different columns

### How to Solve
1. Start with single-cell cages - those digits are fixed immediately
2. Look for cages where only one combination of digits is possible
3. Use row/column constraints to eliminate possibilities
4. When stuck, use logical deduction across intersecting rows, columns and cages

---

## Features

### Puzzle Generation
- Fully **random puzzle generator** - every game is a new puzzle
- Supports **4×4, 5×5 and 6×6** grid sizes
- Uses **Latin Square construction** with row, column and value shuffling for true randomness
- Cage generation uses a **greedy neighbor-grouping algorithm**
- Operators and targets are computed automatically from the solution

### Difficulty Levels
- **Easy** — smaller grid, simpler cages, more single-cell and addition cages
- **Medium** — mixed operators, moderate cage sizes
- **Hard** — larger grid, more multiplication and division cages

### Puzzle Solver
- Built-in **backtracking solver** with **constraint propagation**
- Models the puzzle as a **Constraint Satisfaction Problem (CSP)**
- Prunes invalid branches early using partial cage checks
- Can auto-solve any generated puzzle instantly via the `[A]` command

### Interactive Gameplay
- Clean **console UI** with colored output (ANSI colors)
- **Two-grid display** - left grid shows cage layout by number, right grid shows your answers
- **Cage information panel** explaining every cage's cells, operator and target
- **Live stats bar** showing elapsed time, move count and hints used

### Input Validation
- Detects **row and column conflicts** immediately when you enter a value
- Prevents **overwriting a filled cell** - must clear it first
- Handles **invalid input** (letters where numbers expected, unknown commands) gracefully

### Hint System
- `[H]` reveals one correct cell by solving a copy of the current board
- Hint count is tracked and shown in the stats bar

### Undo
- `[U]` undoes the last move - works for both placing and clearing values
- Full move history maintained using a **stack**

### Save and Load
- `[S]` saves your current mid-game progress to a `.txt` file in the `puzzles/` folder
- Load a saved game from the main menu to resume exactly where you left off
- File format stores grid size, all cage definitions and current board state

### Best Time Tracking
- Best solve time is recorded **per grid size** across all sessions
- Stored persistently in `puzzles/besttimes.txt`
- On solving: shows if you set a **new record**, or how many seconds away you were
- First solve of a size is always recorded automatically

---

## Project Structure

```
kenken/
│
├── src/
│   ├── main.cpp            — Entry point, main menu loop, Windows terminal setup
│   ├── cage.h / cage.cpp   — Cage class: cells, operator, target, constraint check
│   ├── grid.h / grid.cpp   — Grid class: board, cage registry, display, validation
│   ├── solver.h/solver.cpp — Backtracking solver with CSP constraint propagation
│   ├── generator.h/generator.cpp — Random puzzle generator (Latin Square + cage partitioning)
│   ├── validator.h/validator.cpp — Solution validator (rows, columns, cages)
│   └── game.h / game.cpp   — Game session: loop, input, undo, hints, save/load, best times
│
├── puzzles/                — Saved puzzle files and best times stored here
├── .gitignore
└── README.md
```

---

## How to Compile

Make sure you have **g++ with C++17 support** installed.

```bash
g++ -std=c++17 src/main.cpp src/grid.cpp src/cage.cpp src/solver.cpp \
    src/validator.cpp src/generator.cpp src/game.cpp -o kenken
```

On Windows (PowerShell or Command Prompt):
```bash
g++ -std=c++17 src/main.cpp src/grid.cpp src/cage.cpp src/solver.cpp src/validator.cpp src/generator.cpp src/game.cpp -o kenken
```

---

## How to Play

```bash
./kenken
```

On Windows:
```bash
.\kenken
```

You will see the main menu:
```
  +======================================+
  |         KENKEN PUZZLE GAME           |
  +======================================+

  [1]  Play new puzzle
  [2]  Load puzzle from file
  [3]  Exit
```

Select `[1]` to start, choose your grid size and difficulty, and the puzzle is generated instantly.

---

## Commands

These commands are available at any point during gameplay:

| Key | Action | Details |
|-----|--------|---------|
| `E` | **Enter a value** | Prompts: `Row Col Value` - place a digit on the board |
| `C` | **Clear a cell** | Prompts: `Row Col` - removes the value from a cell |
| `U` | **Undo** | Reverses your last action (place or clear) |
| `H` | **Hint** | Reveals one correct cell - increments hint counter |
| `A` | **Auto-solve** | Solves the entire puzzle using the built-in solver |
| `S` | **Save progress** | Saves current board state to a file for later |
| `Q` | **Quit to menu** | Returns to the main menu without losing progress warning |

### Entering values
- Row and column are **0-indexed** - top-left cell is `(0, 0)`
- For a 4×4 grid: rows and columns are `0, 1, 2, 3`
- Example: `E` → `0 2 3` places the digit `3` at row 0, column 2

### Clearing a cell
- Example: `C` → `0 2` clears whatever is at row 0, column 2
- Clearing is undoable with `[U]`

---

## Sample Gameplay

```
  Time: 00:01:45   Moves: 6   Hints: 0
  ---------------------------------------------

      C0   C1   C2   C3          C0   C1   C2   C3
      -----------------          -----------------
  R0 | 3 | 3 | 1 | 2 |        | .  | .  | 3  | .  |
      -----------------          -----------------
  R1 | 3 | 4 | 5 | 2 |        | 1  | .  | .  | .  |
      -----------------          -----------------
  R2 | 6 | 4 | 5 | 7 |        | .  | 2  | .  | .  |
      -----------------          -----------------
  R3 | 6 | 8 | 7 | 8 |        | .  | .  | .  | 4  |
      -----------------          -----------------

  Cage Information:
  ---------------------------------------------
  Cage 1 → target 6+  |  cells: (0,0),(0,1),(1,0)  |  sum must be 6
  Cage 2 → target 2÷  |  cells: (0,2),(0,3)         |  quotient must be 2
  Cage 3 → target 5+  |  cells: (1,1),(1,2)         |  sum must be 5
  Cage 4 → target 3-  |  cells: (1,3),(2,3)         |  difference must be 3
  ...

  How to read: Left grid = cage numbers, Right grid = your answers
  Cells with the same number belong to the same cage.
  Fill the right grid using row/col — digits 1 to 4 in every row and col.

  Commands:
  [E] Enter value    [C] Clear cell
  [U] Undo           [H] Hint
  [A] Auto-solve     [S] Save progress
  [Q] Quit to menu

  >
```

### Win Screen
```
  +======================================+
  |         PUZZLE SOLVED!               |
  +======================================+

  Time taken : 04:32
  Moves made : 18
  Hints used : 0

  *** NEW BEST TIME for 4x4! You beat the record! ***
```

---

## Technical Concepts

| Concept | Where Used |
|---------|-----------|
| **Object Oriented Programming** | Separate classes for Grid, Cage, Solver, Generator, Game |
| **Backtracking Algorithm** | Core of the Solver - recursively tries values, undoes on failure |
| **Constraint Satisfaction Problem (CSP)** | Cage constraints modelled and checked at each step |
| **Forward Checking / Pruning** | Partial cage checks eliminate invalid branches early |
| **Latin Square Construction** | Generator builds a valid solved board before creating cages |
| **Greedy Algorithm** | Cage partitioning uses greedy neighbor grouping |
| **Stack (STL)** | Undo system stores move history in a `std::stack` |
| **File I/O** | Save/load puzzles and persist best times across sessions |
| **ANSI Escape Codes** | Colored terminal output and screen clearing |
| **STL Containers** | `vector`, `map`, `stack`, `pair`, `set` used throughout |
| **Random Number Generation** | `mt19937` Mersenne Twister for high-quality randomness |
| **Exception / Input Handling** | `cin.clear()` and `cin.ignore()` for robust input recovery |

---

## Yashvi~

Built as a DSA + OOP project in C++ to demonstrate algorithmic thinking,
object-oriented design, and practical software development skills.
