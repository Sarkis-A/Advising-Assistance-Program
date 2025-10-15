#pragma once

#include <string>
#include <vector>
#include <functional>
#include "Course.h"

/**
 * @file HashTable.h
 * @brief Open-addressed hash table keyed by Course::number (linear probing).
 *
 * This container stores Course values directly. Collisions are resolved via
 * linear probing. Keys are the Course catalog identifiers (e.g., "CS101"),
 * taken from Course::number.
 */
class HashTable {
private:
    // Parallel arrays for payload and occupancy flags
    std::vector<Course> table;
    std::vector<bool> occupied;
    unsigned int tableSize;

    /**
     * @brief Compute hash for a string key.
     * @param key Catalog key.
     * @return Bucket index in [0, tableSize).
     */
    unsigned int hash(std::string key);

public:
    /**
     * @brief Construct a table with an initial bucket count.
     * @param size Number of buckets to allocate.
     */
    HashTable(unsigned int size);

    /** Maximum desired load factor before growing. */
    static constexpr double LOAD_FACTOR = 0.7;

    /**
     * @brief Insert or update a Course in the table.
     * @param course Course value to upsert; the key is course.number.
     */
    void Insert(Course course);

    /**
     * @brief Lookup a course by catalog number.
     * @param courseNumber Catalog key to find.
     * @return Matching Course, or a default-constructed Course if not found.
     */
    Course Search(std::string courseNumber);

    /**
     * @brief Print all present courses in storage order (not sorted).
     */
    void PrintAll();

    /**
     * @brief Resize the table to a new bucket count and rehash entries.
     * @param newSize New number of buckets.
     */
    void Resize(unsigned int newSize);

    /**
     * @brief Apply a function to each stored course.
     * @param fn Function to apply to each occupied slot.
     */
    void ForEach(const std::function<void(const Course&)>& fn) const;

    /** @return Current capacity (bucket count). */
    size_t Capacity() const;

    /** @return Current number of occupied buckets. */
    size_t Size() const;
};
