#include "FileLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

/**
 * @brief Count the number of lines in a text file.
 *        Used to size structures or report progress only.
 */
namespace {
    int countLinesInFile(const string& fileName) {
        ifstream file(fileName);
        string line;
        int lineCount = 0;
        while (getline(file, line)) {
            ++lineCount;
        }
        return lineCount;
    }
}

/**
 * @brief Load courses into a HashTable from a CSV-like file.
 *        Format: COURSE_NUMBER,COURSE_TITLE[,PREREQ_1,PREREQ_2,...]
 * @note Skips lines with missing number or title and prints a warning.
 */
void loadCourses(HashTable& courseTable, const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: Could not open file: " << fileName << endl;
        return;
    }

    string line;
    int lineNumber = 0;
    while (getline(file, line)) {
        ++lineNumber;
        if (line.empty()) continue;

        istringstream ss(line);
        string courseNumber;
        string courseName;
        vector<string> prerequisites;
        string token;

        // Parse number and title (required)
        if (!getline(ss, courseNumber, ',')) continue;
        if (!getline(ss, courseName, ',')) continue;

        // Validate required fields
        if (courseNumber.empty() || courseName.empty()) {
            cout << "Warning: Line " << lineNumber
                << " skipped due to incorrect formatting." << endl;
            continue;
        }

        // Parse any remaining comma-separated values as prerequisites
        while (getline(ss, token, ',')) {
            if (!token.empty()) prerequisites.push_back(token);
        }

		// Insert new Course into the HashTable
        courseTable.Insert(Course(courseNumber, courseName, prerequisites));
    }

    cout << "Courses loaded successfully (HashTable)." << endl;
}

/**
 * @brief Load courses into a RedBlackTree from a CSV-like file.
 *        Format: COURSE_NUMBER,COURSE_TITLE[,PREREQ_1,PREREQ_2,...]
 */
void loadCourses(RedBlackTree& tree, const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: Could not open file: " << fileName << endl;
        return;
    }

    string line;
    int lineNumber = 0;
    while (getline(file, line)) {
        ++lineNumber;
        if (line.empty()) continue;

        istringstream ss(line);
        string courseNumber;
        string courseName;
        vector<string> prerequisites;
        string token;

        // Parse number and title
        if (!getline(ss, courseNumber, ',')) continue;
        if (!getline(ss, courseName, ',')) continue;

        // Validate required fields
        if (courseNumber.empty() || courseName.empty()) {
            cout << "Warning: Line " << lineNumber
                << " skipped due to incorrect formatting." << endl;
            continue;
        }

        // Parse any remaining comma-separated values as prerequisites
        while (getline(ss, token, ',')) {
            if (!token.empty()) prerequisites.push_back(token);
        }

		// Insert new Course into the RedBlackTree
        tree.Insert(Course(courseNumber, courseName, prerequisites));
    }

    cout << "Courses loaded successfully (RBT)." << endl;
}
