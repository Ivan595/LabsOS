#include "Ls.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>


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
    for (int i = 0; i < *new_argc - 1; i++) {    
        for (int j = 0; j < *new_argc - i - 1; j++) {
            if (strcasecmp(filtered_argv[j], filtered_argv[j + 1]) > 0) {
                char* temp = filtered_argv[j];
                filtered_argv[j] = filtered_argv[j + 1];
                filtered_argv[j + 1] = temp;
            }
        }
    }

    return filtered_argv;
}

int main(int argc, char** argv) {
    opterr = 0;
    bool l = false, a = false;
    char opt;
    while ((opt = getopt(argc, argv, "la")) != -1) {
        switch (opt) {
            case 'l':
                l = true;
                break;
            case 'a':
                a = true;
                break;
            case '?':
                printf("unknown option\n");
                return 1;
        }
    }

    int new_argc;
    char** new_argv = filter_args(argc, argv, &new_argc);

    if (l && a) {
        ls_la(new_argc, new_argv);
    }
    else if (a) {
        ls_a(new_argc, new_argv);
    }
    else if (l) {
        ls_l(new_argc, new_argv);
    }
    else {
        ls(new_argc, new_argv);
    }
    free(new_argv);
    return 0;
}
