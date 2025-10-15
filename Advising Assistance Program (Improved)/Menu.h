#pragma once

#include "RedBlackTree.h"

/**
 * @file Menu.h
 * @brief User-facing menu loop for loading data, printing courses,
 *        looking up course details, and running benchmarks.
 */

 /**
  * Run the main user menu loop.
  * @param courseTree Active Red-Black Tree instance for operations.
  */
void displayMenu(RedBlackTree& courseTree);
