#pragma once

#include <string>
#include <vector>

/**
 * @file Course.h
 * @brief Minimal data model for a university course record.
 *
 * A Course encapsulates a catalog identifier (e.g., "CS101"), a human-readable
 * title, and an ordered list of prerequisite course identifiers. This struct is
 * intentionally lightweight and trivially copyable to make it practical as the
 * value payload in containers such as the open-addressed HashTable and the
 * RedBlackTree used throughout this project.
 */
struct Course {
    /** Catalog key, e.g., "CS101". Used as the unique lookup key in indexes. */
    std::string number;

    /** Human-readable course title, e.g., "Intro to Computer Science". */
    std::string title;

    /** Ordered list of prerequisite course catalog keys. May be empty. */
    std::vector<std::string> prerequisites;

    /** Default-construct an empty Course. Fields are set to empty values. */
    Course();

    /**
     * Construct a Course with explicit fields.
     * @param number        Catalog key.
     * @param title         Course title.
     * @param prerequisites Zero or more prerequisite catalog keys.
     */
    Course(std::string number, std::string title, std::vector<std::string> prerequisites);
};

/**
 * @brief Strict weak ordering by number.
 *
 * Defines ascending order of courses by their catalog key. This enables use of
 * standard algorithms that require ordering (e.g., std::sort). The comparison is
 * case-sensitive and relies on the raw stored number. Callers who need a
 * case-insensitive ordering should normalize keys before constructing Course or
 * use a container that compares case-insensitively.
 */
bool operator<(const Course& a, const Course& b);
