#include "Course.h"

/**
 * Default constructor: initialize an empty/placeholder course. This is returned
 * by search functions when a lookup fails, allowing callers to check for an
 * empty number to detect "not found" without throwing exceptions.
 */
Course::Course() {
    number = "";
    title = "";
}

/**
 * Value constructor: populate the three fields directly. No normalization or
 * validation is performed here by design; the FileLoader is responsible for
 * input sanitation.
 */
Course::Course(std::string number, std::string title, std::vector<std::string> prerequisites) {
    this->number = number;
    this->title = title;
    this->prerequisites = prerequisites;
}

/**
 * Ordering predicate used by ordered algorithms/containers. Courses are ordered
 * lexicographically by their number only; other fields are ignored.
 */
bool operator<(const Course& a, const Course& b) {
    return a.number < b.number;
}
