#ifndef RBTREE_H
#define RBTREE_H

/*
 * Red-Black Tree
 *
 * Self-balancing BST that guarantees O(log n) insert, delete, and search
 * by enforcing four invariants after every structural change:
 *   1. Every node is RED or BLACK.
 *   2. The root is BLACK.
 *   3. No RED node has a RED parent.
 *   4. Every path from a node to a descendant NIL has the same black height.
 *
 * Sentinel NIL node: a single shared BLACK node replaces all NULL leaf
 * pointers. Fixup code reads ->color and ->parent on boundary nodes without
 * NULL checks, simplifying every function that walks or rewires the tree.
 */

#include <stdio.h>
#include <stdlib.h>

typedef enum { RED, BLACK } Color;

typedef struct Node {
    int key;
    Color color;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
} Node;

typedef struct {
    Node *root;  // T->NIL when tree is empty
    Node *NIL;   // sentinel: always BLACK, all pointers self-referential
} RBTree;

/* -------------------- Tree Lifecycle -------------------- */

// Allocates an empty tree and initializes the sentinel NIL node.
RBTree *createTree();

// Frees all nodes (postorder so children are freed before parents), then
// frees the NIL sentinel and the tree struct.
void freeTree(RBTree *T);

/* -------------------- Core Operations -------------------- */

// Standard BST insert followed by fixup to restore RB properties.
// New nodes are colored RED to avoid changing black height — only a
// red-red violation may result, which fixup resolves in O(log n).
// Duplicate keys are ignored to preserve strict BST ordering.
void insert(RBTree *T, int key);

// Removes the node with the given key and restores RB properties via fixup.
// Fixup is only triggered when a BLACK node is removed, since that is the
// only case that reduces black height and violates invariant 4.
// No-op if key is not present.
void deleteNode(RBTree *T, int key);

// Iterative BST search — O(log n) guaranteed by RB balance.
// Returns the matching node or T->NIL if not found.
Node *search(RBTree *T, int key);

/* -------------------- Traversal -------------------- */

// Returns the leftmost (smallest) node in the subtree rooted at x.
Node *minimum(RBTree *T, Node *x);

// Returns the rightmost (largest) node in the subtree rooted at x.
Node *maximum(RBTree *T, Node *x);

// Returns the in-order successor of x (next larger key), or T->NIL if x is
// the maximum. Checks right subtree first, then walks up via parent pointers.
Node *next(RBTree *T, Node *x);

// Returns the in-order predecessor of x (next smaller key), or T->NIL if x
// is the minimum. Mirror of next.
Node *prev(RBTree *T, Node *x);

// Visits nodes left -> node -> right. Produces sorted output on the root.
// Uses a callback so the caller decides what to do with each node.
void inorder(RBTree *T, Node *x, void (*visit)(Node *));

// Visits node -> left -> right. Useful for copying or serializing the tree.
void preorder(RBTree *T, Node *x, void (*visit)(Node *));

// Visits left -> right -> node. Safe order for freeing (children before parent).
void postorder(RBTree *T, Node *x, void (*visit)(Node *));

/* -------------------- Accessors -------------------- */

// Returns the root, or T->NIL if the tree is empty.
Node *getRoot(RBTree *T);

// Returns the parent of x, or T->NIL if x is the root.
Node *getParent(RBTree *T, Node *x);

// Returns the left child of x (may be T->NIL).
Node *getLeft(RBTree *T, Node *x);

// Returns the right child of x (may be T->NIL).
Node *getRight(RBTree *T, Node *x);

/* -------------------- Rotations -------------------- */

// Pivots x down-left: x's right child takes x's place. Preserves BST
// ordering by moving the right child's left subtree to x's right.
void leftRotate(RBTree *T, Node *x);

// Pivots y down-right: y's left child takes y's place. Mirror of leftRotate.
void rightRotate(RBTree *T, Node *y);

#endif
