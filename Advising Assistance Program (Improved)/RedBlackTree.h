#pragma once

#include <string>
#include <vector>
#include <functional>
#include "Course.h"

enum Color { RED, BLACK };

/**
 * @brief Node type for the red-black tree.
 *
 * Each node stores a Course and the metadata/links needed for balancing.
 */
struct RBTNode {
    Course data;     // The stored course record.
    Color color;     // Node color (red or black).
    RBTNode* parent; // Parent pointer.
    RBTNode* left;   // Left child pointer.
    RBTNode* right;  // Right child pointer.

    explicit RBTNode(const Course& c)
        : data(c), color(RED), parent(nullptr), left(nullptr), right(nullptr) {
    }
};

/**
 * @brief Balanced binary search tree (red-black) keyed by Course::number.
 */
class RedBlackTree {
public:
    RedBlackTree();
    ~RedBlackTree();

    /**
     * @brief Insert a course keyed by its catalog number.
     *        If the number already exists, replaces the stored Course.
     */
    void Insert(const Course& course);

    /**
     * @brief Search for a course by catalog number (case-insensitive).
     * @param courseNumber Catalog key to look up.
     * @return Matching Course if found, otherwise a default Course.
     */
    Course Search(std::string courseNumber) const;

    /**
     * @brief Print all courses in ascending order:
     */
    void PrintAll() const;

    /** @brief Remove all nodes from the tree. */
    void Clear();

    /**
     * @brief Apply a function to each Course in ascending order.
     * @param fn Callback invoked with each Course.
     */
    void ForEach(const std::function<void(const Course&)>& fn) const;

    /** @return Number of nodes currently in the tree. */
    size_t Size() const;

private:
    RBTNode* root;

    // Rotations and balancing
    void LeftRotate(RBTNode* pivot);
    void RightRotate(RBTNode* pivot);
    void InsertFixup(RBTNode* newNode);

    // Utility methods
    void InOrderPrint(RBTNode* node) const;
    void Destroy(RBTNode* node);
    void InOrderApply(RBTNode* node, const std::function<void(const Course&)>& fn) const;
    size_t CountNodes(RBTNode* node) const;

    // Compare two keys case-insensitively.
    static int CompareKeys(const std::string& a, const std::string& b);
};
