#pragma once

#include <stdbool.h>

#define DEFAULT_INTEGRITY_BUF_SIZE 5000

typedef void *(*durian_malloc)(size_t);
typedef void (*durian_free)(void *);

struct flags {
    size_t buf_size;
};

struct result {
    char *name;
    bool passed;
};

typedef struct result *(*durian_test)(durian_malloc, durian_free, struct flags *);

struct queue {
    durian_test *fun;
    size_t size;
};
