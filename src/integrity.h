#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "durian.h"
#include "log.h"

#define DEFAULT_INTEGRITY_BUF_SIZE 5000

#define UP(x) x ? x : 8
#define RAND_IN_RANGE(low, high) rand() % (high + 1 - low) + low

#define GUARD_VALUE "dpoison0"

struct result *heap_integrity_check(durian_malloc umalloc, durian_free ufree, struct flags *f);
