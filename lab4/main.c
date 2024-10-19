#include <stdio.h>
#include "Chmod.h"
#include <sys/stat.h>
#include <stdlib.h>

//you can pass several files argv[2], argv[3], ...
//you can pass several masks (delimiter ',') in argv[1]
//supports "^[ugoa]*[+=-]+([rwxXst]*|[ugo])$", "^[+=-]+[0-7]*$", "^[0-7]*$" masks


char** filter_args(int argc, char** argv, int* new_argc) {
    *new_argc = 2;
    for (int i = 2; i < argc; i++) {
        char* file_name = argv[i];
        struct stat file_info;
        if (stat(file_name, &file_info) != 0) {
            printf("my_chmod: невозможно получить доступ к '%s': Нет такого файла или каталога\n", file_name);
        }   
        else {
            (*new_argc)++;
        }
    }
    char** filtered_argv = malloc((*new_argc) * sizeof(char*));
    filtered_argv[0] = argv[0];
    filtered_argv[1] = argv[1];
    int j = 2;
    for (int i = 2; i < argc; i++) {
        
        char* file_name = argv[i];
        struct stat file_info;
        if (stat(file_name, &file_info) == 0) {
            filtered_argv[j] = argv[i];
            j++;
        }   
    }

    return filtered_argv;
}

int main(int argc, char**argv) {
    
    if (argc == 1) {
        printf("my_chmod: пропущен операнд\n");
        return 1;
    }
    if (argc == 2) {
        printf("my_chmod: пропущен операнд после «%s»\n", argv[1]);
        return 1;
    }

    int new_argc;
    char** new_argv = filter_args(argc, argv, &new_argc);
    int res = my_chmod(new_argc, new_argv);
    free(new_argv);
    return res;
}
