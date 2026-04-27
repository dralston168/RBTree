#include "RedBlackTree.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static double elapsed_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

static void shuffle(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

static void runBenchmark(int *keys, int n) {
    struct timespec start, end;
    RBTree *T;
    double ins_ns, srch_ns, del_ns;

    // insert timing
    T = createTree();
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < n; i++)
        insert(T, keys[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);
    ins_ns = elapsed_ns(start, end) / n;
    freeTree(T);

    // search timing — separate pre-built tree so insert cost isn't included
    T = createTree();
    for (int i = 0; i < n; i++)
        insert(T, keys[i]);
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < n; i++)
        search(T, keys[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);
    srch_ns = elapsed_ns(start, end) / n;

    // delete timing — reuse the same pre-built tree
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < n; i++)
        deleteNode(T, keys[i]);
    clock_gettime(CLOCK_MONOTONIC, &end);
    del_ns = elapsed_ns(start, end) / n;
    freeTree(T);

    printf("%-12d  %14.2f  %14.2f  %14.2f\n", n, ins_ns, srch_ns, del_ns);
}

int main() {
    int sizes[] = {1000, 10000, 100000, 1000000};
    int numSizes = 4;

    srand(42);

    printf("=== Sequential Input ===\n");
    printf("%-12s  %14s  %14s  %14s\n", "Size", "Insert(ns/op)", "Search(ns/op)", "Delete(ns/op)");
    for (int s = 0; s < numSizes; s++) {
        int n = sizes[s];
        int *keys = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) keys[i] = i;
        runBenchmark(keys, n);
        free(keys);
    }

    printf("\n=== Random Input ===\n");
    printf("%-12s  %14s  %14s  %14s\n", "Size", "Insert(ns/op)", "Search(ns/op)", "Delete(ns/op)");
    for (int s = 0; s < numSizes; s++) {
        int n = sizes[s];
        int *keys = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) keys[i] = i;
        shuffle(keys, n);
        runBenchmark(keys, n);
        free(keys);
    }

    printf("\n=== Reverse Input ===\n");
    printf("%-12s  %14s  %14s  %14s\n", "Size", "Insert(ns/op)", "Search(ns/op)", "Delete(ns/op)");
    for (int s = 0; s < numSizes; s++) {
        int n = sizes[s];
        int *keys = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) keys[i] = n - 1 - i;
        runBenchmark(keys, n);
        free(keys);
    }

    return 0;
}
