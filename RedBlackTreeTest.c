#include "RedBlackTree.h"
#include <limits.h>

#define TEST(condition, name) \
    if (condition) printf("PASS: %s\n", name); \
    else printf("FAIL: %s (line %d)\n", name, __LINE__);

// verify no red node has a red parent
int checkNoRedRed(RBTree *T, Node *x) {
    if (x == T->NIL) return 1;
    if (x->color == RED && x->parent->color == RED) return 0;
    return checkNoRedRed(T, x->left) && checkNoRedRed(T, x->right);
}

// verify all paths to NIL have same black height
// returns -1 if invalid, otherwise black height
int checkBlackHeight(RBTree *T, Node *x) {
    if (x == T->NIL) return 1;
    int left = checkBlackHeight(T, x->left);
    int right = checkBlackHeight(T, x->right);
    if (left == -1 || right == -1 || left != right) return -1;
    return left + (x->color == BLACK ? 1 : 0);
}

// verify BST ordering property
int checkBST(RBTree *T, Node *x, int min, int max) {
    if (x == T->NIL) return 1;
    if (x->key <= min || x->key >= max) return 0;
    return checkBST(T, x->left, min, x->key) &&
           checkBST(T, x->right, x->key, max);
}

// verify root is black
int checkRoot(RBTree *T) {
    return T->root->color == BLACK;
}

// run all property checks at once
void verifyRBTree(RBTree *T, char *testName) {
    TEST(checkRoot(T), "root is black");
    TEST(checkNoRedRed(T, T->root), "no red-red violation");
    TEST(checkBlackHeight(T, T->root) != -1, "black height consistent");
    TEST(checkBST(T, T->root, INT_MIN, INT_MAX), "BST property holds");
    printf("--- verified after: %s ---\n\n", testName);
}

void testInsert() {
    RBTree *T = createTree();

    insert(T, 10);
    TEST(T->root->key == 10, "single insert becomes root");
    TEST(T->root->color == BLACK, "root is black after insert");
    verifyRBTree(T, "single insert");

    insert(T, 20);
    insert(T, 30);  // triggers rotation
    verifyRBTree(T, "insert triggers rotation");

    insert(T, 15);
    insert(T, 25);
    verifyRBTree(T, "multiple inserts");
    freeTree(T);
}

void testDelete() {
    RBTree *T = createTree();
    int keys[] = {10, 20, 30, 15, 25, 5, 1};
    int n = 7;

    for (int i = 0; i < n; i++)
        insert(T, keys[i]);

    deleteNode(T, 20);  // delete node with two children
    TEST(search(T, 20) == T->NIL, "deleted node not found");
    verifyRBTree(T, "delete two-child node");

    deleteNode(T, 1);   // delete leaf
    TEST(search(T, 1) == T->NIL, "deleted leaf not found");
    verifyRBTree(T, "delete leaf");

    deleteNode(T, 10);  // delete node with one child
    TEST(search(T, 10) == T->NIL, "deleted one-child node not found");
    verifyRBTree(T, "delete one-child node");
    freeTree(T);
}

void testSearch() {
    RBTree *T = createTree();
    insert(T, 10);
    insert(T, 20);
    insert(T, 5);

    TEST(search(T, 10) != T->NIL, "search finds existing node");
    TEST(search(T, 99) == T->NIL, "search returns NIL for missing key");
    freeTree(T);
}

void testNextPrev() {
    RBTree *T = createTree();
    int keys[] = {10, 20, 30, 15, 25};
    for (int i = 0; i < 5; i++)
        insert(T, keys[i]);

    Node *n = search(T, 15);
    TEST(next(T, n)->key == 20, "next of 15 is 20");
    TEST(prev(T, n)->key == 10, "prev of 15 is 10");

    Node *maxNode = maximum(T, T->root);
    TEST(next(T, maxNode) == T->NIL, "next of max is NIL");

    Node *minNode = minimum(T, T->root);
    TEST(prev(T, minNode) == T->NIL, "prev of min is NIL");
    freeTree(T);
}

void testLargeInsertDelete() {
    RBTree *T = createTree();

    // insert 1000 nodes
    for (int i = 0; i < 1000; i++)
        insert(T, i);
    verifyRBTree(T, "1000 sequential inserts");

    // delete every other node
    for (int i = 0; i < 1000; i += 2)
        deleteNode(T, i);
    verifyRBTree(T, "500 deletions");
    freeTree(T);
}

void testRandomInsertDelete() {
    RBTree *T = createTree();
    srand(42);

    for (int i = 0; i < 500; i++)
        insert(T, rand() % 1000);
    verifyRBTree(T, "500 random inserts");

    for (int i = 0; i < 250; i++)
        deleteNode(T, rand() % 1000);  // some may not exist, that's fine
    verifyRBTree(T, "250 random deletes");
    freeTree(T);
}

static int traversalBuf[1024];
static int traversalCount;

static void collectKey(Node *n) {
    traversalBuf[traversalCount++] = n->key;
}

void testMinMax() {
    RBTree *T = createTree();
    int keys[] = {10, 5, 20, 15, 30, 1, 25};
    for (int i = 0; i < 7; i++)
        insert(T, keys[i]);

    TEST(minimum(T, T->root)->key == 1,  "minimum of full tree is 1");
    TEST(maximum(T, T->root)->key == 30, "maximum of full tree is 30");

    Node *sub = search(T, 20);
    TEST(minimum(T, sub)->key == 15, "minimum of subtree rooted at 20 is 15");
    TEST(maximum(T, sub)->key == 30, "maximum of subtree rooted at 20 is 30");

    freeTree(T);
}

void testTraversal() {
    RBTree *T = createTree();
    int keys[] = {10, 5, 20, 15, 30, 1, 25};
    int n = 7;
    for (int i = 0; i < n; i++)
        insert(T, keys[i]);

    // inorder must visit all nodes in sorted order
    traversalCount = 0;
    inorder(T, T->root, collectKey);
    TEST(traversalCount == n, "inorder visits all nodes");
    int sorted = 1;
    for (int i = 1; i < traversalCount; i++) {
        if (traversalBuf[i] <= traversalBuf[i-1]) { sorted = 0; break; }
    }
    TEST(sorted, "inorder output is sorted");

    // preorder and postorder must also visit all nodes
    traversalCount = 0;
    preorder(T, T->root, collectKey);
    TEST(traversalCount == n, "preorder visits all nodes");

    traversalCount = 0;
    postorder(T, T->root, collectKey);
    TEST(traversalCount == n, "postorder visits all nodes");

    freeTree(T);
}

void testEdgeCases() {
    RBTree *T = createTree();

    // delete from empty tree
    deleteNode(T, 10);  // should not crash
    TEST(T->root == T->NIL, "delete from empty tree safe");

    // duplicate insert
    insert(T, 10);
    insert(T, 10);
    verifyRBTree(T, "duplicate insert");

    // delete root
    insert(T, 5);
    deleteNode(T, 10);
    verifyRBTree(T, "delete root");
    freeTree(T);
}

int main() {
    printf("=== Unit Tests ===\n");
    testInsert();
    testDelete();
    testSearch();
    testNextPrev();
    testMinMax();
    testTraversal();

    printf("=== Integration Tests ===\n");
    testLargeInsertDelete();
    testRandomInsertDelete();

    printf("=== Edge Cases ===\n");
    testEdgeCases();

    return 0;
}