/*
Created by Austin Sarkis
CS 300
10/17/24
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <limits>
#include <fstream>
#include <sstream>

using namespace std;

const double LOAD_FACTOR = 0.7;
const unsigned int INITIAL_SIZE = 5;  // Initial hash table size

// Structure to hold course information
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;

    // Default constructor
    Course() {
        courseNumber = "";
        courseName = "";
    }

    // Parameterized constructor
    Course(string number, string name, vector<string> prereqs) {
        courseNumber = number;
        courseName = name;
        prerequisites = prereqs;
    }
};

// Overload < operator for Course to compare courseNumber
bool operator<(const Course& a, const Course& b) {
    return a.courseNumber < b.courseNumber;
}

// Defines the HashTable class
class HashTable {
private:
    vector<Course> table;       // Store courses directly in the hash table
    vector<bool> occupied;      // Tracks if a slot is occupied
    unsigned int tableSize;     // Current size of hash table

    unsigned int hash(string key); // Hash function

public:
    HashTable(unsigned int size);
    void Insert(Course course);
    void PrintAll();
    Course Search(string courseNumber);
    void Resize(unsigned int newSize);
};

// HashTable constructor with dynamic size
HashTable::HashTable(unsigned int size) {
    tableSize = size;
    table.resize(tableSize);            // Resize the table to the initial size
    occupied.resize(tableSize, false);  // Mark all slots as unoccupied initially
}

// Hash function (convert the course number to a numeric hash)
unsigned int HashTable::hash(string key) {
    int sum = 0;

    for (char ch : key) {
        sum += static_cast<int>(ch);
    }

    return sum % tableSize;
}

// Insert a course into the hash table using linear probing
void HashTable::Insert(Course course) {
    unsigned int key = hash(course.courseNumber);  // Compute hash for the course
    unsigned int originalKey = key;                // Store the original hash key for probing

    // Finds the next available slot if there's a collision
    while (occupied[key]) {
        key = (key + 1) % tableSize;  // Move to the next slot (and wrap around)

        if (key == originalKey) {
            cout << "Error: Hash table is full." << endl;
            return;
        }
    }

    table[key] = course;   // Insert the course in the available slot
    occupied[key] = true;  // Mark the slot as occupied
}

// Prints all courses in alphanumeric order
void HashTable::PrintAll() {
    vector<Course> courses;  // Temporary vector to store courses

    // Collect all occupied courses from the hash table
    for (unsigned int i = 0; i < tableSize; ++i) {
        if (occupied[i]) {
            courses.push_back(table[i]);
        }
    }

    // Sorts the courses (Uses overloaded < operator)
    sort(courses.begin(), courses.end());

    // Print the sorted courses
    for (const Course& course : courses) {
        cout << course.courseNumber << ", " << course.courseName << endl;
    }
}

// Function to search for a course by course number using linear probing
Course HashTable::Search(string courseNumber) {
    // Convert the courseNumber input to uppercase
    transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);

    unsigned int key = hash(courseNumber);  // Compute hash for the course
    unsigned int originalKey = key;         // Store the original hash key for probing

    // Search for the course
    while (occupied[key]) {
        if (table[key].courseNumber == courseNumber) {
            return table[key];  // Return the course if found
        }

        key = (key + 1) % tableSize;  // Move to the next slot (and wrap around)

        if (key == originalKey) {
            break;  // Looped back to the start, course not found
        }
    }

    return Course();  // Return an empty course if not found
}

// Function to resize the hash table (clears and resizes the table)
void HashTable::Resize(unsigned int newSize) {
    tableSize = newSize;
    table.clear();
    occupied.clear();
    table.resize(tableSize);
    occupied.resize(tableSize, false);  // Mark all slots as unoccupied initially
}

// Count the number of lines in the file
int countLinesInFile(const string& fileName) {
    ifstream file(fileName);
    string line;
    int lineCount = 0;

    while (getline(file, line)) {
        lineCount++;
    }

    return lineCount;
}

// Function to load courses from a file with resizing
void loadCourses(HashTable& courseTable, const string& fileName) {
    ifstream file(fileName);

    // Check if the file exists and can be opened
    if (!file.is_open()) {
        cout << "Error: Could not open file.\n" << endl;
        return;
    }

    // Counts the number of lines in the file
    string line;
    int lineCount = 0;
    while (getline(file, line)) {
        lineCount++;
    }

    // If there are no lines, return
    if (lineCount == 0) {
        cout << "Error: No courses found in the file.\n" << endl;
        return;
    }

    unsigned int newTableSize = static_cast<unsigned int>(lineCount / LOAD_FACTOR);  // Calculates the new table size based on the load factor

    courseTable.Resize(newTableSize);  // Resizes the hash table to the new size

    // Reset the file to read the data again
    file.clear();  // Clear EOF flag
    file.seekg(0); // Move back to the start of the file

    int lineNumber = 0; // Keeps track of line numbers

    // Load courses from the file
    while (getline(file, line)) {
        lineNumber++;
        stringstream ss(line);
        string courseNumber, courseName;
        vector<string> prerequisites;
        string token;

        // Parse course number and course name
        getline(ss, courseNumber, ',');
        getline(ss, courseName, ',');

        // Check if either courseNumber or courseName is missing
        if (courseNumber.empty() || courseName.empty()) {
            cout << "Warning: Line " << lineNumber << " skipped due to incorrect formatting." << endl;
            continue; // Skip to the next line if the format is incorrect
        }

        // Parse prerequisites (if any)
        while (getline(ss, token, ',')) {
            if (!token.empty()) {
                prerequisites.push_back(token);
            }
        }

        // Insert course into the hash table
        Course course(courseNumber, courseName, prerequisites);
        courseTable.Insert(course);
    }

    cout << "Courses loaded successfully.\n" << endl;
}


// Input validation function
int getValidatedInput() {
    int choice;

    while (!(cin >> choice)) {
        cout << "\nInvalid input. Please enter a number." << endl;
        cin.clear(); // Clear the error flag on cin
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer after valid input

    return choice;
}

// Menu function
void displayMenu(HashTable& courseTable) {
    int choice = 0;

    while (choice != 9) {
        cout << "Menu Options:" << endl;
        cout << "1. Load courses from file" << endl;
        cout << "2. Print all courses" << endl;
        cout << "3. Print course information" << endl;
        cout << "9. Exit" << endl;
        cout << "Enter your choice: ";

        choice = getValidatedInput();  // Pervents infitnite looping if input is not an int

        switch (choice) {
        case 1: {
            string fileName;
            cout << "Enter the filename: ";
            cin >> fileName;
            loadCourses(courseTable, fileName);
            break;
        }

        case 2:
            cout << "Here is a sample schedule:\n" << endl;
            courseTable.PrintAll();
            cout << endl;
            break;

        case 3: {
            string courseNumber;
            cout << "Enter the course number you are looking for: ";
            cin >> courseNumber;
            cout << endl;

            Course course = courseTable.Search(courseNumber);

            if (!course.courseNumber.empty()) {
                cout << "Course: " << course.courseNumber << ", " << course.courseName << endl;
                cout << "Prerequisites: ";

                // Show 'None' if there are no prerequisites
                if (course.prerequisites.empty()) {
                    cout << "None" << endl;
                }
                else {
                    for (const string& prereq : course.prerequisites) {
                        cout << prereq << " ";
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

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << "\nInvalid option. Please try again.\n" << endl;
        }
    }
}

int main() {
    // Declare hash table with small initial size
    HashTable courseTable(INITIAL_SIZE);

    cout << "Welcome to the course planner.\n" << endl;

    displayMenu(courseTable);
    return 0;
}