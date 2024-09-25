#include "Cat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void cat(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        FILE* file;
        char* line = NULL;
        size_t len = 0;
        file = fopen(argv[i], "r");
        if (file == NULL) {
            fprintf(stderr, "Ошибка при открытии файла\n");
            return;   
        }   
        while (getline(&line, &len, file) != -1) {
            printf("%s", line);
        }
        free(line);
        fclose(file);
    } 
}

void cat_E(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        FILE* file;
        char* line = NULL;
        size_t len = 0;
        file = fopen(argv[i], "r");
        if (file == NULL) {
            fprintf(stderr, "Ошибка при открытии файла\n");
            return;   
        }   
        while (getline(&line, &len, file) != -1) {
            line[strlen(line) - 1] = '$';
            printf("%s\n", line);
        }
        free(line);
        fclose(file);
    }
}

void cat_n(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        FILE* file;
        char* line = NULL;
        size_t len = 0;
        file = fopen(argv[i], "r");
        if (file == NULL) {
            fprintf(stderr, "Ошибка при открытии файла\n");
            return;   
        }   
        int num = 1;
        while (getline(&line, &len, file) != -1) {
            printf("%6d  %s", num, line);
            num++;
        }
        free(line);
        fclose(file);
    } 
}

void cat_b(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        FILE* file;
        char* line = NULL;
        size_t len = 0;
        file = fopen(argv[i], "r");
        if (file == NULL) {
            fprintf(stderr, "Ошибка при открытии файла\n");
            return;   
        }   
        int num = 1;
        while (getline(&line, &len, file) != -1) {
            if (strlen(line) != 1) {
                printf("%6d  %s", num, line);
                num++;
            }
            else {
                printf("\n");   
            }  
        }
        free(line);
        fclose(file);
    } 
}

void cat_bE(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        FILE* file;
        char* line = NULL;
        size_t len = 0;
        file = fopen(argv[i], "r");
        if (file == NULL) {
            fprintf(stderr, "Ошибка при открытии файла\n");
            return;   
        }   
        int num = 1;
        while (getline(&line, &len, file) != -1) {
            line[strlen(line) - 1] = '$';
            if (strlen(line) != 1) {
                printf("%6d  %s\n", num, line);
                num++;
            }
            else {
                printf("%s\n", line);
            }
        }
        free(line);
        fclose(file);
    } 
}

void cat_nE(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        FILE* file;
        char* line = NULL;
        size_t len = 0;
        file = fopen(argv[i], "r");
        if (file == NULL) {
            fprintf(stderr, "Ошибка при открытии файла\n");
            return;   
        }   
        int num = 1;
        while (getline(&line, &len, file) != -1) {
            line[strlen(line) - 1] = '$';
            printf("%6d  %s\n", num, line);
            num++;
        }
        free(line);
        fclose(file);
    } 
}

