# Red-Black Tree

A Red-Black Tree implemented from scratch in C, supporting insert, delete, and search with guaranteed O(log n) performance enforced by structural rebalancing after every mutation.

---

## Files

| File | Description |
|------|-------------|
| `RedBlackTree.h` | Public API: structs, constants, and function declarations |
| `RedBlackTree.c` | Full implementation of all operations |
| `RedBlackTreeTest.c` | Unit, integration, and edge-case test suite |
| `RedBlackTreePerf.c` | Timing benchmarks across sequential, random, and reverse input |

---

## Compile & Run

**Test suite:**
```
gcc -O2 -o rbt_test RedBlackTree.c RedBlackTreeTest.c && ./rbt_test
```

**Performance benchmarks:**
```
gcc -O2 -o rbt_perf RedBlackTree.c RedBlackTreePerf.c && ./rbt_perf
```

---

## API

```c
RBTree *createTree();                        // allocate an empty tree
void    freeTree(RBTree *T);                 // free all nodes and the tree

void    insert(RBTree *T, int key);          // insert key (no-op on duplicate)
void    deleteNode(RBTree *T, int key);      // remove key (no-op if absent)
Node   *search(RBTree *T, int key);          // returns node or T->NIL if missing

Node   *minimum(RBTree *T, Node *x);        // leftmost node in subtree
Node   *maximum(RBTree *T, Node *x);        // rightmost node in subtree
Node   *next(RBTree *T, Node *x);           // in-order successor
Node   *prev(RBTree *T, Node *x);           // in-order predecessor

void    inorder (RBTree *T, Node *x, void (*visit)(Node *));
void    preorder (RBTree *T, Node *x, void (*visit)(Node *));
void    postorder(RBTree *T, Node *x, void (*visit)(Node *));
```

All functions that can reach a boundary return `T->NIL` (never `NULL`), so callers compare against `T->NIL` uniformly without NULL checks.

---

## Usage Example

```c
RBTree *T = createTree();

insert(T, 10);
insert(T, 20);
insert(T, 5);
insert(T, 10);   // duplicate — silently ignored

Node *n = search(T, 20);
if (n != T->NIL)
    printf("Found: %d\n", n->key);   // Found: 20

n = search(T, 99);
if (n == T->NIL)
    printf("Not found\n");           // Not found

deleteNode(T, 20);
printf("After delete: %s\n",
    search(T, 20) == T->NIL ? "gone" : "still there");  // gone

freeTree(T);
```

---

## Sample Test Output

```
=== Unit Tests ===
PASS: single insert becomes root
PASS: root is black after insert
PASS: root is black
PASS: no red-red violation
PASS: black height consistent
PASS: BST property holds
--- verified after: single insert ---

PASS: root is black
PASS: no red-red violation
PASS: black height consistent
PASS: BST property holds
--- verified after: insert triggers rotation ---

PASS: deleted node not found
PASS: root is black
PASS: no red-red violation
PASS: black height consistent
PASS: BST property holds
--- verified after: delete two-child node ---

PASS: search finds existing node
PASS: search returns NIL for missing key
PASS: next of 15 is 20
PASS: prev of 15 is 10
PASS: next of max is NIL
PASS: prev of min is NIL

=== Integration Tests ===
PASS: root is black
PASS: no red-red violation
PASS: black height consistent
PASS: BST property holds
--- verified after: 1000 sequential inserts ---

PASS: root is black
PASS: no red-red violation
PASS: black height consistent
PASS: BST property holds
--- verified after: 500 deletions ---

=== Edge Cases ===
PASS: delete from empty tree safe
PASS: root is black
PASS: no red-red violation
PASS: black height consistent
PASS: BST property holds
--- verified after: duplicate insert ---

PASS: root is black
PASS: no red-red violation
PASS: black height consistent
PASS: BST property holds
--- verified after: delete root ---
```

---

## Sample Performance Output

```
=== Sequential Input ===
Size           Insert(ns/op)   Search(ns/op)   Delete(ns/op)
1000                  114.00           20.00           67.00
10000                 145.90           22.20           82.70
100000                233.16           17.65           58.37
1000000               201.57           22.16           67.36

=== Random Input ===
Size           Insert(ns/op)   Search(ns/op)   Delete(ns/op)
1000                   75.00           20.00           59.00
10000                  96.70           31.80           73.10
100000                132.09           47.05          107.77
1000000               231.56           78.67          188.80

=== Reverse Input ===
Size           Insert(ns/op)   Search(ns/op)   Delete(ns/op)
1000                   67.00           10.00           40.00
10000                 106.90           11.50           43.90
100000                190.29           15.47           64.72
1000000               203.73           21.50           71.57
```

All operations scale logarithmically
