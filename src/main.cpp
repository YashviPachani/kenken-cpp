#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#include <iostream>
#include "game.h"

#ifdef _WIN32
#include <windows.h>
#endif



using namespace std;

int main() {
     // Enable UTF-8 and ANSI colors on Windows terminal
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    #endif
    Game game;
    int choice = 0;

    while (choice != 3) {
        game.showMainMenu();
        cin >> choice;

        if (choice == 1)      game.startNewGame();
        else if (choice == 2) {
            cout << "  Enter filename to load: ";
            string fname;
            cin >> fname;
            game.loadPuzzle(fname);
        }
        else if (choice == 3) {
            cout << "\n  \033[32mGoodbye!\033[0m\n\n";
        }
    }

    return 0;
}