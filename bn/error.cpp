#include <bn/error.hpp>

namespace bn {

void
error(const char *fmt, ...) {
    fflush(stderr);
    fflush(stdout);
    fprintf(stderr, "error: ");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    fflush(stderr);
    exit(1);
}

};
