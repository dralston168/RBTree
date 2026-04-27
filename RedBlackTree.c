#include <stdio.h>
#include <stdlib.h>
#include "RedBlackTree.h"

/* -------------------- Function Prototypes -------------------- */
Node* createNode(RBTree *T, int key);
RBTree* createTree();

void leftRotate(RBTree *T, Node *x);
void rightRotate(RBTree *T, Node *y);

void insert(RBTree *T, int key);
void insertFixup(RBTree *T, Node *z);
Node* insertFixupA(RBTree *T, Node *z);
void insertFixupB(RBTree *T, Node *z);
void insertFixupC(RBTree *T, Node *z);

Node* search(RBTree *T, int key);
Node* minimum(RBTree *T, Node *node);

void transplant(RBTree *T, Node *u, Node *v);
void deleteNode(RBTree *T, int key);
void deleteFixup(RBTree *T, Node *x);

void freeTree(RBTree *T);
void inorder(RBTree *T, Node *x, void (*visit)(Node *));
void preorder(RBTree *T, Node *x, void (*visit)(Node *));
void postorder(RBTree *T, Node *x, void (*visit)(Node *));

/* -------------------- Tree Initialization -------------------- */
// The sentinel NIL node stands in for all NULL leaf boundaries. It is always
// BLACK with self-referential pointers so fixup code can read ->color and
// ->parent at any boundary without a NULL check.
RBTree* createTree() {
    RBTree *T = (RBTree*)malloc(sizeof(RBTree));

    T->NIL = (Node*)malloc(sizeof(Node));
    T->NIL->color = BLACK;
    T->NIL->left = T->NIL;
    T->NIL->right = T->NIL;
    T->NIL->parent = T->NIL;

    T->root = T->NIL;

    return T;
}

/* -------------------- Node Creation -------------------- */
// New nodes are always RED so insertion never changes black height.
// Only a red-red violation can result, which is cheaper to fix than
// a black height imbalance.
Node* createNode(RBTree *T, int key) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->color = RED;
    node->left = T->NIL;
    node->right = T->NIL;
    node->parent = T->NIL;
    return node;
}

/* -------------------- Search -------------------- */
// Iterative BST walk — avoids recursion overhead on deep trees.
// Returns T->NIL on miss so callers use one uniform comparison.
Node* search(RBTree *T, int key) {
    Node *current = T->root;
    while (current != T->NIL) {
        if (key == current->key)
            return current;
        else if (key < current->key)
            current = current->left;
        else
            current = current->right;
    }
    return T->NIL;
}

Node *getRoot(RBTree *T) {
    return T->root;
}

// Returns T->NIL if x is the root so callers get a safe sentinel, not NULL.
Node *getParent(RBTree *T, Node *x) {
    if (x == T->root)
        return T->NIL;
    return x->parent;
}

Node *getLeft(RBTree *T, Node *x) {
    return x->left;
}

Node *getRight(RBTree *T, Node *x) {
    return x->right;
}

// Follows left pointers to the bottom — leftmost node is always the smallest.
Node *minimum(RBTree *T, Node *x) {
    while (x->left != T->NIL)
        x = x->left;
    return x;
}

// Follows right pointers to the bottom — rightmost node is always the largest.
Node *maximum(RBTree *T, Node *x) {
    while (x->right != T->NIL)
        x = x->right;
    return x;
}

// If x has a right subtree, successor is its minimum (leftmost of right).
// Otherwise walk up until we take a left-child edge — that ancestor is next.
Node *next(RBTree *T, Node *x) {
    if (x->right != T->NIL)
        return minimum(T, x->right);
    Node *y = x->parent;
    while (y != T->NIL && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

// Mirror of next: right subtree max, or walk up until a right-child edge.
Node *prev(RBTree *T, Node *x) {
    if (x->left != T->NIL)
        return maximum(T, x->left);
    Node *y = x->parent;
    while (y != T->NIL && x == y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

/* -------------------- Rotations -------------------- */
// Pivots x down-left. y's left subtree keys all fall between x and y, so
// moving that subtree to x's right maintains BST ordering at O(1) cost.
void leftRotate(RBTree *T, Node *x) {
    Node *y = x->right;

    x->right = y->left;
    if (y->left != T->NIL)
        y->left->parent = x;

    y->parent = x->parent;

    if (x->parent == T->NIL)
        T->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

// Mirror of leftRotate — pivots y down-right.
void rightRotate(RBTree *T, Node *y) {
    Node *x = y->left;

    y->left = x->right;
    if (x->right != T->NIL)
        x->right->parent = y;

    x->parent = y->parent;

    if (y->parent == T->NIL)
        T->root = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left = x;

    x->right = y;
    y->parent = x;
}

// Replaces the subtree rooted at u with the subtree rooted at v.
// Only rewires the parent link — the caller must update v's children.
void transplant(RBTree *T, Node *u, Node *v) {
    if (u->parent == T->NIL)
        T->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    v->parent = u->parent;
}

/* -------------------- Insert -------------------- */
void insert(RBTree *T, int key) {
    if (search(T, key) != T->NIL) return;
    Node *newNode = createNode(T, key);
    Node *parent = T->NIL;
    Node *root = T->root;

    while (root != T->NIL) {
        parent = root;
        if (newNode->key < root->key)
            root = root->left;
        else
            root = root->right;
    }

    newNode->parent = parent;

    if (parent == T->NIL)
        T->root = newNode;
    else if (newNode->key < parent->key)
        parent->left = newNode;
    else
        parent->right = newNode;

    newNode->left = T->NIL;
    newNode->right = T->NIL;
    newNode->color = RED;

    insertFixup(T, newNode);
}

// Case 1: uncle is RED — recolor parent and uncle BLACK, grandparent RED,
// then move up to grandparent and repeat. Pushes the red-red violation upward
// without changing black heights. Returns the node where the loop stopped
// so insertFixupB/C know where to continue — C passes node by value so any
// internal changes to node would be lost without returning it.
Node* insertFixupA(RBTree *T, Node *node) {
    while (node != T->root && node->parent->color == RED) {
        Node *parent = node->parent;
        Node *grandparent = parent->parent;
        Node *uncle = (parent == grandparent->left) ? grandparent->right : grandparent->left;

        if (uncle->color == RED) {
            node->parent->color = BLACK;
            uncle->color = BLACK;
            grandparent->color = RED;
            node = grandparent;
        } else {
            break;
        }
    }
    return node;
}

// Case 2: uncle is BLACK, triangle shape (node is inner grandchild — left-right
// or right-left). Rotate the parent to straighten into a line, then fall
// through to Case 3. One rotation converts triangle to line shape.
void insertFixupB(RBTree *T, Node *node) {
    if (node->parent->color == RED) {
        Node *parent = node->parent;
        Node *grandparent = parent->parent;

        if (parent == grandparent->left && node == parent->right) {
            leftRotate(T, parent);
            node = parent;
            parent = node->parent;
        } else if (parent == grandparent->right && node == parent->left) {
            rightRotate(T, parent);
            node = parent;
            parent = node->parent;
        }
        insertFixupC(T, node);
    }
}

// Case 3: uncle is BLACK, line shape (node is outer grandchild — left-left
// or right-right). Rotate grandparent and recolor to restore invariant 3.
// This is the final step — one rotation and two recolors terminates fixup.
void insertFixupC(RBTree *T, Node *node) {
    if (node->parent->color == RED) {
        Node *parent = node->parent;
        Node *grandparent = parent->parent;

        if (parent == grandparent->left && node == parent->left) {
            rightRotate(T, grandparent);
            parent->color = BLACK;
            grandparent->color = RED;
        } else if (parent == grandparent->right && node == parent->right) {
            leftRotate(T, grandparent);
            parent->color = BLACK;
            grandparent->color = RED;
        }
    }
}

// Chains A -> B -> C. A's return value is captured because it walks node
// up the tree during recoloring — without capturing it, B and C would
// operate on the original inserted node, which may already be resolved.
// Root is forced BLACK at the end to always satisfy invariant 2.
void insertFixup(RBTree *T, Node *node) {
    node = insertFixupA(T, node);
    insertFixupB(T, node);
    T->root->color = BLACK;
}

/* -------------------- Delete -------------------- */
void deleteNode(RBTree *T, int key) {
    Node *z = T->root;

    while (z != T->NIL) {
        if (key == z->key) break;
        else if (key < z->key) z = z->left;
        else z = z->right;
    }
    if (z == T->NIL) return;

    Node *y = z;
    Node *x;
    // Fixup is only needed when a BLACK node is removed — that is the only
    // case that reduces black height and violates invariant 4.
    Color yOriginalColor = y->color;

    if (z->left == T->NIL) {
        x = z->right;
        transplant(T, z, z->right);
    } else if (z->right == T->NIL) {
        x = z->left;
        transplant(T, z, z->left);
    } else {
        // Two children: replace z with its in-order successor (leftmost of
        // right subtree). The successor is the next larger key so placing it
        // at z maintains BST ordering without touching other nodes.
        y = minimum(T, z->right);
        yOriginalColor = y->color;
        x = y->right;

        if (y->parent == z) {
            // x may be T->NIL — set its parent explicitly so deleteFixup
            // can walk up via x->parent correctly.
            x->parent = y;
        } else {
            transplant(T, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        transplant(T, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    free(z);

    if (yOriginalColor == BLACK)
        deleteFixup(T, x);
}

// Case 1: sibling is RED — rotate parent toward x and recolor, exposing a
// BLACK sibling. Converts to Case 2, 3, or 4 without changing black height.
void deleteFixupA(RBTree *T, Node *x) {
    Node *parent = x->parent;
    Node *sibling;
    if (x == parent->left) {
        sibling = parent->right;
        sibling->color = BLACK;
        parent->color = RED;
        leftRotate(T, parent);
    } else {
        sibling = parent->left;
        sibling->color = BLACK;
        parent->color = RED;
        rightRotate(T, parent);
    }
}

// Case 2: sibling is BLACK, both sibling children BLACK — recolor sibling RED
// to balance the local black height, then propagate the deficit up to parent.
Node *deleteFixupB(RBTree *T, Node *x) {
    Node *parent = x->parent;
    Node *sibling = (x == parent->left) ? parent->right : parent->left;
    sibling->color = RED;
    return parent;
}

// Case 3: sibling is BLACK, near child RED, far child BLACK — rotate sibling
// away from x and recolor to make the far child RED, setting up Case 4.
void deleteFixupC(RBTree *T, Node *x) {
    Node *parent = x->parent;
    Node *sibling;
    if (x == parent->left) {
        sibling = parent->right;
        sibling->color = RED;
        sibling->left->color = BLACK;
        rightRotate(T, sibling);
    } else {
        sibling = parent->left;
        sibling->color = RED;
        sibling->right->color = BLACK;
        leftRotate(T, sibling);
    }
}

// Case 4: sibling is BLACK, far child RED — rotate parent toward x and
// recolor to restore black height. This terminates fixup in one step.
Node *deleteFixupD(RBTree *T, Node *x) {
    Node *parent = x->parent;
    Node *sibling;
    if (x == parent->left) {
        sibling = parent->right;
        sibling->color = parent->color;
        parent->color = BLACK;
        sibling->right->color = BLACK;
        leftRotate(T, parent);
    } else {
        sibling = parent->left;
        sibling->color = parent->color;
        parent->color = BLACK;
        sibling->left->color = BLACK;
        rightRotate(T, parent);
    }
    return T->root;
}

void deleteFixup(RBTree *T, Node *x) {
    while (x != T->root && x->color == BLACK) {
        Node *parent = x->parent;
        Node *sibling;

        if (x == parent->left)
            sibling = parent->right;
        else
            sibling = parent->left;

        if (sibling->color == RED) {
            deleteFixupA(T, x);
            parent = x->parent;
            sibling = (x == parent->left) ? parent->right : parent->left;
        }

        if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
            x = deleteFixupB(T, x);
        } else {
            if ((x == parent->left && sibling->left->color == RED && sibling->right->color == BLACK) ||
                (x == parent->right && sibling->right->color == RED && sibling->left->color == BLACK)) {
                deleteFixupC(T, x);
                sibling = (x == parent->left) ? parent->right : parent->left;
            }
            x = deleteFixupD(T, x);
            break;
        }
    }

    x->color = BLACK;
}

/* -------------------- Traversals -------------------- */
void inorder(RBTree *T, Node *x, void (*visit)(Node *)) {
    if (x == T->NIL) return;
    inorder(T, x->left, visit);
    visit(x);
    inorder(T, x->right, visit);
}

void preorder(RBTree *T, Node *x, void (*visit)(Node *)) {
    if (x == T->NIL) return;
    visit(x);
    preorder(T, x->left, visit);
    preorder(T, x->right, visit);
}

void postorder(RBTree *T, Node *x, void (*visit)(Node *)) {
    if (x == T->NIL) return;
    postorder(T, x->left, visit);
    postorder(T, x->right, visit);
    visit(x);
}

/* -------------------- Cleanup -------------------- */
// Postorder traversal frees children before the parent, ensuring no node
// is freed while its children's pointers are still needed.
static void freeNodes(RBTree *T, Node *x) {
    if (x == T->NIL) return;
    freeNodes(T, x->left);
    freeNodes(T, x->right);
    free(x);
}

void freeTree(RBTree *T) {
    freeNodes(T, T->root);
    free(T->NIL);
    free(T);
}
