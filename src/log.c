#include "log.h"

/* Q&D logging function...
   Really bad honestly, this is not an example of what
   a proper logging function looks like... */
void pr_log(const char *format, ...)
{
    time_t timer;
    char tm_buf[26];
    char *buf = malloc(500); /* 500 is random... */
    struct tm* tm_info;
    timer = time(NULL);
    tm_info = localtime(&timer);
    strftime(tm_buf, 26, "%d-%m-%Y %H:%M:%S", tm_info);
    va_list arg;
    va_start(arg, format);
    vsprintf(buf, format, arg);
    printf("%s: %s", tm_buf, buf);
    va_end(arg);
    free(buf);
}
