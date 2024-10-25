#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include "Archiver.h"
#include <stdlib.h>

int main(int argc, char** argv) {

    struct option longopts[] = {
        {"help", 0, NULL, 'h'},
        {"extract", 0, NULL, 'e'},
        {"input", 0, NULL, 'i'},
        {"stat", 0, NULL, 's'},
        {NULL, 0, NULL, 0}
    };

    opterr = 0;
    char opt;
    bool h = false;
    bool e = false;
    bool i = false;
    bool s = false;
    int count = 0;
    while ((opt = getopt_long(argc, argv, "heis", longopts, NULL)) != -1) {
        switch (opt) {
            case 'h':
                h = true;
                count++;
                break;
            case 'e':
                e = true;
                count++;
                break;
            case 'i':
                i = true;
                count++;
                break;
            case 's':
                s = true;
                count++;
                break;
            case '?':
                printf("Unknown option\n");
                return 1;
        }
    }
    
    if (count > 1) {
        printf("wrong use\n");
    }

    if (h) {
        if (argc != 2) {
            printf("Неверные аргументы\n");
            return 1;
        }
        help();
    }
    if (i) {
        if (argc != 4) {
            printf("Неверные аргументы\n");
            return 1;
        }
        input(argv[2], argv[3]);
    }
    if (s) {
        if (argc != 3) {
            printf("Неверные аргументы\n");
            return 1;
        }
        archive_stat(argv[2]);
    }
    if (e) {
        if (argc != 4) {
            printf("Неверные аргументы\n");
            return 1;
        }
        extract(argv[2], argv[3]);
    }



    return 0;
}
