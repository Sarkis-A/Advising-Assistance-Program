#include "Menu.h"
#include <iostream>
using namespace std;

/**
 * Entry point for the course planner.
 * Initializes the active data structure (Red-Black Tree) and
 * hands off to the interactive menu.
 */
int main() {
	RedBlackTree courseTree;  // the main data structure for base program
    cout << "Welcome to the course planner.\n" << endl;
    displayMenu(courseTree);  // pass RBT into the menu loop
    return 0;
}
