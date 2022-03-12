#include "integrity.h"

uint64_t rand64(void)
{
    return rand();
}

void bitwise_copy(char *str, uint64_t *bits)
{
    for (int i = 7; i >= 0; i--) {
        *bits <<= 8;
        *bits |= (uint64_t) str[i];
    }
}

struct result *heap_integrity_check(durian_malloc umalloc, durian_free ufree, struct flags *f)
{
    struct result *res = malloc(sizeof(struct result));
    uint64_t **buf = malloc(sizeof(uint64_t *) * f->buf_size);
    srand(time(NULL));
    res->name = "heap integrity check";
    res->passed = true;

    /* Fill buffer with random bytes,
       only occasionally poisoning with guard values */
    unsigned int j = 0;
    int psn_modulo = RAND_IN_RANGE(2, 3);
    unsigned int *guards = malloc((f->buf_size / psn_modulo) * sizeof(unsigned int) + 1);
    for (unsigned int i = 0; i < f->buf_size; i++) {
        buf[i] = umalloc(sizeof(uint64_t));
        if (i % psn_modulo == 0) {
            /* Plant guard value */
            bitwise_copy(GUARD_VALUE, buf[i]);
            guards[j++] = i;
        } else {
            *buf[i] = rand64();
        }
    }

    /* Now let's check if our buffer is intact! */
    uint64_t poison;
    bitwise_copy(GUARD_VALUE, &poison);
    for (unsigned int i = 0; i < j; i++) {
        const uint64_t val = *buf[guards[i]];
        if (val != poison) {
            /* The heap is smashed... */
            pr_log(
                "Memory corruption detected! (at %p with incorrect bytes: 0x%lx)\n",
                &buf[guards[i]],
                val
            );
            res->passed = false;
        }
    }

    for (unsigned int i = 0; i < f->buf_size; i++) {
        ufree(buf[i]);
    }

    free(buf);
    free(guards);
    return res;
}
