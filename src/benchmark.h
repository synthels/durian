#pragma once

#include <time.h>

#include "durian.h"
#include "log.h"

struct result *malloc_benchmark(durian_malloc umalloc, durian_free ufree, struct flags *f);
