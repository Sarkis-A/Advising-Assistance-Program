#include "Menu.h"
#include "FileLoader.h"
#include "Benchmark.h"
#include <iostream>
#include <limits>
#include <string>
using namespace std;

namespace {

    /**
     * Read an integer choice from std::cin with validation.
     * Discards bad input and leaves the stream ready for next read.
     */
    int getValidatedInput() {
        int choice;
        while (!(cin >> choice)) {
            cout << "\nInvalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return choice;
    }

    /**
     * Prompt for a size_t value with a default. Empty input returns defaultVal.
     */
    size_t getValidatedSizeT(const string& prompt, size_t defaultVal) {
        cout << prompt << " [" << defaultVal << "]: ";
        string line;
        getline(cin, line);
        if (line.empty()) return defaultVal;
        try {
            return static_cast<size_t>(stoull(line));
        }
        catch (...) {
            cout << "Invalid input. Using default " << defaultVal << "." << endl;
            return defaultVal;
        }
    }

    /**
     * Prompt for a double value with a default. Empty input returns defaultVal.
     */
    double getValidatedDouble(const string& prompt, double defaultVal) {
        cout << prompt << " [" << defaultVal << "]: ";
        string line;
        getline(cin, line);
        if (line.empty()) return defaultVal;
        try {
            return stod(line);
        }
        catch (...) {
            cout << "Invalid input. Using default " << defaultVal << "." << endl;
            return defaultVal;
        }
    }

    /**
     * Pretty-print a single benchmark result block.
     */
    void printBench(const BenchResult& r) {
        cout << "\n=== Benchmark Results ===" << endl;
        cout << "Dataset:          " << r.datasetName << endl;
        cout << "Courses:          " << r.numCourses << endl;
        cout << "Search trials:    " << r.numSearchTrials << endl;
        cout << "Build time (ms):  " << r.buildMs << endl;
        cout << "Hit search (ms):  " << r.searchHitMs << endl;
        cout << "Miss search (ms): " << r.searchMissMs << endl;
        cout << "Mixed (ms):       " << r.mixedMs << endl;
        cout << "Range (ms):       " << r.rangeMs << endl;
        cout << "=========================\n" << endl;
    }

    /**
     * Show a side-by-side summary for HashTable vs. RedBlackTree.
     */
    void printBenchComparison(const BenchResult& benchHT, const BenchResult& benchRBT) {
        cout << "\n=== Side-by-Side ===\n";
        cout << "Dataset: " << benchHT.datasetName
            << " (Courses: " << benchHT.numCourses
            << ", Trials: " << benchHT.numSearchTrials << ")\n";
        cout << "Build (ms):       HT=" << benchHT.buildMs << "   RBT=" << benchRBT.buildMs << "\n";
        cout << "Hit search (ms):  HT=" << benchHT.searchHitMs << "   RBT=" << benchRBT.searchHitMs << "\n";
        cout << "Miss search (ms): HT=" << benchHT.searchMissMs << "   RBT=" << benchRBT.searchMissMs << "\n";
        cout << "Mixed (ms):       HT=" << benchHT.mixedMs << "   RBT=" << benchRBT.mixedMs << "\n";
        cout << "Range (ms):       HT=" << benchHT.rangeMs << "   RBT=" << benchRBT.rangeMs << "\n";
        cout << "====================\n" << endl;
    }

} // namespace

/**
 * Display the interactive menu and route user actions.
 * Operations act on the provided Red-Black Tree instance.
 */
void displayMenu(RedBlackTree& courseTree) {
    int choice = 0;

    while (choice != 9) {
        cout << "Menu Options:\n"
            << "1. Load courses from file\n"
            << "2. Print all courses\n"
            << "3. Print course information\n"
            << "4. Run benchmarks (HT / RBT / Both)\n"
            << "9. Exit\n"
            << "Enter your choice: ";

        choice = getValidatedInput();

        switch (choice) {
        case 1: {
            string fileName;
            cout << "Enter the filename: ";
            getline(cin >> ws, fileName);

            courseTree.Clear();
            loadCourses(courseTree, fileName);  // RBT overload
            break;
        }
        case 2: {
            cout << "Here is a sample schedule:\n" << endl;
            courseTree.PrintAll();
            cout << endl;
            break;
        }
        case 3: {
            string courseNumber;
            cout << "Enter the course number you are looking for: ";
            getline(cin >> ws, courseNumber);
            cout << endl;

            // Search is case-insensitive internally
            Course course = courseTree.Search(courseNumber);
            if (!course.number.empty()) {
                cout << "Course: " << course.number
                    << ", " << course.title << endl;
                cout << "Prerequisites: ";
                if (course.prerequisites.empty()) {
                    cout << "None" << endl;
                }
                else {
                    for (const auto& p : course.prerequisites) {
                        cout << p << " ";
                    }
                    cout << endl;
                }
                cout << endl;
            }
            else {
                cout << "Course not found.\n" << endl;
            }
            break;
        }
        case 4: {
            string fileName;
            cout << "Enter dataset filename for benchmark: ";
            getline(cin >> ws, fileName);

            cout << "Select data structure: 1) HashTable  2) RedBlackTree  3) Both  [3]: ";
            string dsChoiceLine;
            getline(cin, dsChoiceLine);
            int dsChoice = dsChoiceLine.empty() ? 3 : stoi(dsChoiceLine);

            size_t trials = getValidatedSizeT("Number of search trials (per phase)", 5000);
            double hitRatio = getValidatedDouble("Mixed workload hit ratio (0..1)", 0.5);
            if (hitRatio < 0.0) hitRatio = 0.0;
            if (hitRatio > 1.0) hitRatio = 1.0;

            string prefix = "CS2";
            cout << "Range/prefix to test (e.g., CS2) [CS]: ";
            {
                string line; getline(cin, line);
                if (!line.empty()) prefix = line;
            }

            if (dsChoice == 1) {
                BenchResult a = RunHashTableBenchmark(fileName, trials, hitRatio, prefix);
                printBench(a);
            }
            else if (dsChoice == 2) {
                BenchResult b = RunRBTBenchmark(fileName, trials, hitRatio, prefix);
                printBench(b);
            }
            else {
                BenchResult a = RunHashTableBenchmark(fileName, trials, hitRatio, prefix);
                BenchResult b = RunRBTBenchmark(fileName, trials, hitRatio, prefix);
                printBenchComparison(a, b);
            }
            break;
        }
        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;
        default:
            cout << "\nInvalid option. Please try again.\n" << endl;
        }
    }
}
