#include <stdio.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"


int main(int argc, char **argv) {
    const char *program_name = shift(argv, argc);
    printf("Hello nobby world! From %s\n", program_name);

    return 0;
}
