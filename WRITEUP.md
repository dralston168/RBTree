# Red-Black Tree — Explanation

## Overview

A Red-Black Tree is a self-balancing BST that enforces four invariants after every structural change:

1. Every node is RED or BLACK.
2. The root is BLACK.
3. No RED node has a RED parent.
4. Every path from any node to a NIL boundary crosses the same number of BLACK nodes.

These invariants bound tree height at 2 log₂(n+1), keeping insert, delete, and search at O(log n) worst case.

---

## Sentinel NIL Node

All leaf edges point to a single shared sentinel (`T->NIL`) with `color = BLACK` and all pointers looping back to itself. This lets fixup code read `->color`, `->parent`, `->left`, and `->right` at any boundary without NULL checks — `T->NIL->color == BLACK` always, so checks like `sibling->left->color == BLACK` are safe even when the sibling's children are the sentinel.

---

## Search

Iterative BST walk from the root: go left if the key is smaller, right if larger, return the node on a match, or `T->NIL` if the bottom is reached. No rebalancing needed — the RB invariants keep depth bounded at O(log n).

---

## Insert

**BST step:** Walk left/right from the root until a NIL boundary is reached and attach the new node there. New nodes are RED so black height is unchanged — only a red-red violation can result. Duplicates are silently ignored.

**Fixup** inspects the uncle's color and proceeds in three cases:

- **Case A — uncle RED:** Recolor parent and uncle BLACK, grandparent RED. Repeat at grandparent; walks upward until the parent is BLACK or the root is reached.
- **Case B — uncle BLACK, triangle shape:** Rotate the parent to straighten the path into a line, then fall through to C.
- **Case C — uncle BLACK, line shape:** Rotate the grandparent away and recolor (former parent → BLACK, former grandparent → RED). Terminates fixup.

The root is forced BLACK after fixup.

---

## Delete

**BST step:** Three structural cases:

- **0 or 1 child:** Splice `z` out by transplanting its one non-NIL child (or `T->NIL`) into its place.
- **2 children:** Replace `z` with its in-order successor `y` (leftmost of right subtree). Splice `y` out, copy `z`'s color to `y`, and use `y`'s former right child as fixup starting point `x`.

**Fixup** is only triggered when the removed node was BLACK — removing a RED node leaves black heights unchanged. The algorithm carries a conceptual extra unit of blackness at `x` and iterates until absorbed:

- **Case A — sibling RED:** Rotate parent toward `x`, swap parent/sibling colors. Gives `x` a BLACK sibling; converts to B, C, or D.
- **Case B — sibling BLACK, both children BLACK:** Recolor sibling RED, move `x` up to parent. Deficit propagates upward.
- **Case C — sibling BLACK, near child RED, far child BLACK:** Rotate sibling away from `x`, swap sibling/near-child colors. Far child is now RED; fall through to D.
- **Case D — sibling BLACK, far child RED:** Rotate parent toward `x`. Give sibling the parent's color, color parent and far child BLACK. Restores black height; terminates fixup.

After the loop, `x->color = BLACK` absorbs any remaining deficit.

---

## Left and Right Rotations

Rotations are O(1) pointer rewirings that reshape the tree without breaking BST ordering.

**Left rotation on x** — `x`'s right child `y` takes `x`'s place; `y`'s former left subtree moves to `x`'s right:

```
    x                y
   / \              / \
  A   y    →      x   C
     / \         / \
    B   C       A   B
```

**Right rotation on y** is the exact mirror.

---

## Coloring Rules Summary

| Situation | Action |
|-----------|--------|
| New node inserted | Always colored RED |
| Root (after any fixup) | Forced BLACK |
| Insert Case A (RED uncle) | Parent → BLACK, uncle → BLACK, grandparent → RED |
| Insert Case C (BLACK uncle, line) | Former parent → BLACK, former grandparent → RED |
| Delete Case A (RED sibling) | Sibling → BLACK, parent → RED |
| Delete Case B (BLACK sibling, both children BLACK) | Sibling → RED |
| Delete Case D (BLACK sibling, far child RED) | Sibling → parent's color, parent → BLACK, far child → BLACK |

Sentinel `T->NIL` is always BLACK and never recolored.
