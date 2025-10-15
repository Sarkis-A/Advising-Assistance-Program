#pragma once

#include <string>
#include "HashTable.h"
#include "RedBlackTree.h"

/**
 * @file FileLoader.h
 * @brief Functions to load Course records from a CSV-like file into data structures.
 *
 * Expected file format per line:
 *   COURSE_NUMBER,COURSE_TITLE[,PREREQ_1,PREREQ_2,...]
 *
 * Behavior:
 *  - Lines with missing number or title are skipped with a warning.
 *  - Prerequisites are optional; any remaining comma-separated values on the line
 *    are treated as Course prerequisites.
 *  - No exceptions are thrown for malformed lines; loading proceeds line-by-line.
 */

 /**
  * @brief Load courses into a HashTable.
  * @param courseTable Destination hash table.
  * @param fileName    Path to the input file.
  */
void loadCourses(HashTable& courseTable, const std::string& fileName);

/**
 * @brief Load courses into a RedBlackTree.
 * @param tree     Destination red-black tree.
 * @param fileName Path to the input file.
 */
void loadCourses(RedBlackTree& tree, const std::string& fileName);
