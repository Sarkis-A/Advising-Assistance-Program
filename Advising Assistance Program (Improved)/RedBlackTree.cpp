#include "RedBlackTree.h"
#include <algorithm>
#include <cctype>
#include <iostream>
using namespace std;

// --- Helpers ---
/**
 * @brief Return an uppercase copy of the input string.
 */
static string ToUpperCopy(string s) {
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

/**
 * @brief Compare two string keys case-insensitively.
 * @return -1 if a < b, 1 if a > b, 0 if equal.
 */
int RedBlackTree::CompareKeys(const string& a, const string& b) {
    string A = ToUpperCopy(a);
    string B = ToUpperCopy(b);
    if (A < B) return -1;
    if (A > B) return 1;
    return 0;
}

// --- Lifecycle ---
RedBlackTree::RedBlackTree() : root(nullptr) {}

RedBlackTree::~RedBlackTree() {
    Destroy(root);
    root = nullptr;
}

void RedBlackTree::Destroy(RBTNode* node) {
    if (!node) return;
    // Recurse left, then right, then delete this node
    Destroy(node->left);
    Destroy(node->right);
    delete node;
}

void RedBlackTree::Clear() {
    Destroy(root);
    root = nullptr;
}

// --- Rotations ---
/**
 * @brief Left-rotate the subtree rooted at 'pivot'.
 */
void RedBlackTree::LeftRotate(RBTNode* pivot) {
    RBTNode* child = pivot->right;
    if (!child) return;

    // Move child’s left subtree into pivot’s right slot
    pivot->right = child->left;
    if (child->left) child->left->parent = pivot;

    // Link child to pivot’s parent
    child->parent = pivot->parent;
    if (!pivot->parent) {
        root = child;
    }
    else if (pivot == pivot->parent->left) {
        pivot->parent->left = child;
    }
    else {
        pivot->parent->right = child;
    }

    // Make pivot the left child of its former right child
    child->left = pivot;
    pivot->parent = child;
}

/**
 * @brief Right-rotate the subtree rooted at 'pivot'.
 */
void RedBlackTree::RightRotate(RBTNode* pivot) {
    RBTNode* child = pivot->left;
    if (!child) return;

    // Move child’s right subtree into pivot’s left slot
    pivot->left = child->right;
    if (child->right) child->right->parent = pivot;

    // Link child to pivot’s parent
    child->parent = pivot->parent;
    if (!pivot->parent) {
        root = child;
    }
    else if (pivot == pivot->parent->left) {
        pivot->parent->left = child;
    }
    else {
        pivot->parent->right = child;
    }

    // Make pivot the right child of its former left child
    child->right = pivot;
    pivot->parent = child;
}

// --- Insert + Fixup ---
/**
 * @brief Insert a Course into the tree keyed by Course::number.
 *        Duplicate keys replace the stored payload.
 */
void RedBlackTree::Insert(const Course& course) {
    RBTNode* newNode = new RBTNode(course);
    RBTNode* parentNode = nullptr;
    RBTNode* currentNode = root;

    // Traverse down the tree like a BST
    while (currentNode != nullptr) {
        parentNode = currentNode;
        int cmp = CompareKeys(newNode->data.number, currentNode->data.number);
        if (cmp == 0) {
            // Replace existing course payload if duplicate found
            currentNode->data = course;
            delete newNode;
            return;
        }
        else if (cmp < 0) {
            currentNode = currentNode->left;
        }
        else {
            currentNode = currentNode->right;
        }
    }

    // Attach new node to parent
    newNode->parent = parentNode;
    if (parentNode == nullptr) {
        root = newNode; // tree was empty
    }
    else if (CompareKeys(newNode->data.number, parentNode->data.number) < 0) {
        parentNode->left = newNode;
    }
    else {
        parentNode->right = newNode;
    }

    // Restore red-black invariants
    InsertFixup(newNode);
}

/**
 * @brief Restore red-black invariants after insertion.
 */
void RedBlackTree::InsertFixup(RBTNode* newNode) {
    while (newNode->parent && newNode->parent->color == RED) {
        RBTNode* grandParent = newNode->parent->parent;
        if (!grandParent) break;

        if (newNode->parent == grandParent->left) {
            RBTNode* uncleNode = grandParent->right;
            if (uncleNode && uncleNode->color == RED) {
                // Recolor parent + uncle, push red up
                newNode->parent->color = BLACK;
                uncleNode->color = BLACK;
                grandParent->color = RED;
                newNode = grandParent;
            }
            else {
                if (newNode == newNode->parent->right) {
                    newNode = newNode->parent;
                    LeftRotate(newNode);
                }
                newNode->parent->color = BLACK;
                grandParent->color = RED;
                RightRotate(grandParent);
            }
        }
        else {
            // Mirror logic when parent is a right child
            RBTNode* uncleNode = grandParent->left;
            if (uncleNode && uncleNode->color == RED) {
                newNode->parent->color = BLACK;
                uncleNode->color = BLACK;
                grandParent->color = RED;
                newNode = grandParent;
            }
            else {
                if (newNode == newNode->parent->left) {
                    newNode = newNode->parent;
                    RightRotate(newNode);
                }
                newNode->parent->color = BLACK;
                grandParent->color = RED;
                LeftRotate(grandParent);
            }
        }
    }
    if (root) root->color = BLACK; // root must always be black
}

// --- Search ---
/**
 * @brief Case-insensitive search for a Course by catalog number.
 */
Course RedBlackTree::Search(string courseNumber) const {
    courseNumber = ToUpperCopy(courseNumber);
    RBTNode* currentNode = root;

	// Standard BST search but no recursive calls
    while (currentNode) {
        string curKey = ToUpperCopy(currentNode->data.number);
        if (courseNumber == curKey) {
            return currentNode->data;
        }
        else if (courseNumber < curKey) {
            currentNode = currentNode->left;
        }
        else {
            currentNode = currentNode->right;
        }
    }
    return Course(); // not found
}

// --- Print ---
/**
 * @brief Recursive in-order traversal that prints "NUMBER, TITLE".
 */
void RedBlackTree::InOrderPrint(RBTNode* node) const {
    if (!node) return;
    InOrderPrint(node->left);
    cout << node->data.number << ", " << node->data.title << endl;
    InOrderPrint(node->right);
}

void RedBlackTree::PrintAll() const {
    InOrderPrint(root);
}

void RedBlackTree::ForEach(const function<void(const Course&)>& fn) const {
    InOrderApply(root, fn);
}

void RedBlackTree::InOrderApply(RBTNode* node, const function<void(const Course&)>& fn) const {
    if (!node) return;
    InOrderApply(node->left, fn);
    fn(node->data);
    InOrderApply(node->right, fn);
}

/**
 * @brief Recursively count nodes in a subtree.
 */
size_t RedBlackTree::CountNodes(RBTNode* node) const {
    if (!node) return 0;
    return 1 + CountNodes(node->left) + CountNodes(node->right);
}

size_t RedBlackTree::Size() const {
    return CountNodes(root);
}
