#include "benchmark.h"

#define BK_ALLOCATION_SIZE 200
#define BK_RERUNS 1000000

struct result *malloc_benchmark(durian_malloc umalloc, durian_free ufree, struct flags *f)
{
    struct result *res = malloc(sizeof(struct result));
    res->name = "allocation benchmark";
    res->passed = true;

    double avg = 0;
    for (int i = 0; i < BK_RERUNS; i++) {
        clock_t start = clock();
        ufree(umalloc(BK_ALLOCATION_SIZE));
        avg += (clock() - start);
    }
    pr_log("Average allocation time: %fms\n", ((avg / BK_RERUNS) / (CLOCKS_PER_SEC / 1000)));
    return res;
}
