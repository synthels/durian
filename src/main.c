#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <dlfcn.h>

#include "integrity.h"
#include "benchmark.h"
#include "log.h"

#define NUMBER_OF_TESTS 2
#define __WELCOME_TEXT "durian (C) synthels 2022\n"

void sigsegv_handler(int signum, siginfo_t *info, void *c)
{
    pr_log("It looks like your malloc caused a segmentation fault! That can't be a good sign! Consider fixing that!\n");
    exit(1);
}

void show_help_message(void)
{
    fprintf(stdout, 
        "Usage: durian [options]\n"
        " Run various tests in order to determine if a malloc implementation is up to standard.\n"
        "Options:\n"
        " -l, --library\t\toverride malloc with another shared library\n"
        " -f, --file\t\toutput file\n"
        " -i, --check-integrity\tcheck heap integrity\n"
        " -b, --benchmark\tbenchmark allocation time\n"
        " -h, --help\t\tdisplay this help message\n"
    );
}

void add_function_if_not_already_in(struct queue *queue, durian_test test)
{
    for (unsigned int i = 0; i < queue->size; i++) {
        if (queue->fun[i] == test) {
            return;
        }
    }

    queue->fun[queue->size++] = test;
}

struct queue *queue_get(void)
{
    struct queue *q = malloc(sizeof(struct queue));
    q->fun = malloc(NUMBER_OF_TESTS * sizeof(durian_test));
    q->size = 0;
    return q;
}

void free_queue(struct queue *q)
{
    free(q->fun);
    free(q);
}

void show_results(bool is_stdout, struct result *res)
{
    if (is_stdout)
        pr_log("\e[1m%s \e[0m%s\n", res->passed ? "\e[92mPassed" : "\e[91Failed", res->name);
    else
        pr_log("%s %s\n", res->passed ? "Passed" : "Failed", res->name);
}

int main(int argc, char **argv)
{
    struct option args[] = {
        { "library", required_argument, NULL, 'l' },
        { "file", required_argument, NULL, 'f' },
        { "check-integrity", optional_argument, NULL, 'i' },
        { "benchmark", no_argument, NULL, 'b' },
        { "help", no_argument, NULL, 'h' },
        { 0, 0, 0, 0 }
    };

    if (argc < 2) {
        show_help_message();
        return 1;
    }

    /* Install a SIGSEGV handler */
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = sigsegv_handler;
    sigaction(SIGSEGV, &action, NULL);

    char ch;
    char *library = NULL;
    char *file = NULL;
    struct queue *q = queue_get();
    struct flags f;
    while ((ch = getopt_long(argc, argv, "l:hbf:i::", args, NULL)) != -1) {
        switch (ch) {
            case 'l':
                library = optarg;
                break;
            case 'i':
                add_function_if_not_already_in(q, heap_integrity_check);
                f.buf_size = optarg ? atoll(optarg) : DEFAULT_INTEGRITY_BUF_SIZE;
                break;
            case 'b':
                add_function_if_not_already_in(q, malloc_benchmark);
                break;
            case 'f':
                file = optarg;
                break;
            case 'h':
            default:
                show_help_message();
                free_queue(q);
                return 0;
        }
    }

    pr_log(__WELCOME_TEXT);

    /* If user specified a library, load it! */
    durian_malloc umalloc = malloc;
    durian_free ufree = free;
    void *lib = NULL;
    if (library) {
        pr_log("Opening library \"%s\"...\n", library);
        lib = dlopen(library, RTLD_LAZY);
        if (lib) {
            *(void **)(&umalloc) = dlsym(lib, "malloc");
            *(void **)(&ufree) = dlsym(lib, "free");
            if (!(umalloc && ufree)) {
                fprintf(stdout, "Couldn't find malloc or free in library \"%s\"!\n", library);
            }
        } else {
            fprintf(stdout, "Couldn't load library! (%s)\n", dlerror());
            free_queue(q);
            return 1;
        }
    } else {
        pr_log("No library specified, defaulting to malloc/free.\n");
    }

    /* Redirect stdout, if requested */
    bool is_stdout = true;
    if (file) {
        /* NOTE: if I'm not mistaken, you don't have to close freopen'd stdout... */
        freopen(file, "wa+", stdout);
        is_stdout = false;
    }

    /* Run tests */
    for (unsigned int i = 0; i < q->size; i++) {
        struct result *res = (q->fun[i])(umalloc, ufree, &f);
        show_results(is_stdout, res);
        free(res);
    }

    pr_log("All tests done!\n");

    free_queue(q);
    if (lib) {
        /* Free the library, if we ever used one */
        dlclose(lib);
    }
}
