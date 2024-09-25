#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Cat.h"

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
    opterr = 0;
    bool n = false, b = false, E = false;
    char opt;
    while ((opt = getopt(argc, argv, "nbE")) != -1) {
        switch (opt) {
            case 'n':
                n = true;
                break;
            case 'b':
                b = true;
                break;
            case 'E':
                E = true;
                break;
            case '?':
                printf("unknown option\n");
                return 1;
        }
    }

    int new_argc;
    char** new_argv = filter_args(argc, argv, &new_argc);
    if (n && b && E){
        cat_bE(new_argc, new_argv);
    }
    else if (n && b) {
        cat_b(new_argc, new_argv);
    }
    else if (n && E) {
        cat_nE(new_argc, new_argv);
    }
    else if (b && E) {
        cat_bE(new_argc, new_argv);
    }
    else if (n) {
        cat_n(new_argc, new_argv);
    }
    else if (b) {
        cat_b(new_argc, new_argv);
    }
    else if (E) {
        cat_E(new_argc, new_argv);
    }
    else {
        cat(new_argc, new_argv);
    }
    free(new_argv);
    return 0;
}
