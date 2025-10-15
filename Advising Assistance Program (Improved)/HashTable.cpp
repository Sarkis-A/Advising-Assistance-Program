#include "HashTable.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <functional>
using namespace std;

/**
 * @brief Construct a hash table with a given number of buckets.
 * @param size Number of buckets to allocate.
 */
HashTable::HashTable(unsigned int size) {
    tableSize = size;
    table.resize(tableSize);
    occupied.resize(tableSize, false);
}

/**
 * @brief Compute a simple hash value for a string key.
 * @param key Input string (typically a Course catalog number).
 * @return Index in the range [0, tableSize).
 */
unsigned int HashTable::hash(string key) {
    int sum = 0;
    for (char ch : key) {
        sum += static_cast<int>(ch);
    }
    return sum % tableSize;
}

/**
 * @brief Insert a new course into the table.
 *        Uses linear probing to resolve collisions.
 * @param course The Course object to insert.
 */
void HashTable::Insert(Course course) {
    unsigned int key = hash(course.number);
    unsigned int originalKey = key;

    while (occupied[key]) {
        key = (key + 1) % tableSize;
        if (key == originalKey) {
            cout << "Error: Hash table is full." << endl;
            return;
        }
    }

    table[key] = course;
    occupied[key] = true;
}

/**
 * @brief Search for a course by its catalog number.
 * @param courseNumber The course key to find (case-insensitive).
 * @return Matching Course if found, otherwise a default-constructed Course.
 */
Course HashTable::Search(string courseNumber) {
    transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);

    unsigned int key = hash(courseNumber);
    unsigned int originalKey = key;

    while (occupied[key]) {
        if (table[key].number == courseNumber) {
            return table[key];
        }
        key = (key + 1) % tableSize;
        if (key == originalKey) break;
    }
    return Course();
}

/**
 * @brief Print all courses in sorted order by course number.
 */
void HashTable::PrintAll() {
    vector<Course> courses;
    for (unsigned int i = 0; i < tableSize; ++i) {
        if (occupied[i]) courses.push_back(table[i]);
    }
    sort(courses.begin(), courses.end());
    for (const auto& c : courses) {
        cout << c.number << ", " << c.title << endl;
    }
}

/**
 * @brief Resize the table to a new bucket count.
 *        Clears the table and resets occupancy flags.
 * @param newSize The new bucket count.
 */
void HashTable::Resize(unsigned int newSize) {
    tableSize = newSize;
    table.clear();
    occupied.clear();
    table.resize(tableSize);
    occupied.resize(tableSize, false);
}

/**
 * @brief Apply a function to each stored Course.
 * @param fn Function to invoke for every occupied slot.
 */
void HashTable::ForEach(const function<void(const Course&)>& fn) const {
    for (unsigned int i = 0; i < tableSize; ++i) {
        if (occupied[i]) fn(table[i]);
    }
}

/** @return The number of buckets currently allocated. */
size_t HashTable::Capacity() const {
    return static_cast<size_t>(tableSize);
}

/** @return The number of occupied buckets. */
size_t HashTable::Size() const {
    size_t n = 0;
    for (unsigned int i = 0; i < tableSize; ++i) if (occupied[i]) ++n;
    return n;
}
