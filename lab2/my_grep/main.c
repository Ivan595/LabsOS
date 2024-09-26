#include "Grep.h" //some text
#include <stdlib.h>

char** filter_args(int argc, char** argv, int* new_argc) {
    *new_argc = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            (*new_argc)++;
        }
    }
    char** filtered_argv = malloc((*new_argc) * sizeof(char*));
    int j = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            filtered_argv[j] = argv[i];
            j++;
        }
    }
    return filtered_argv;
}

int main(int argc, char** argv) {
    int rv, new_argc;
    char** new_argv = filter_args(argc, argv, &new_argc);
    rv = grep(new_argc, new_argv);
    free(new_argv);
    return rv;
}
