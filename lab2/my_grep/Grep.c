#include "Grep.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define RESET "\033[0m"
#define RED "\033[31;1m"

int process_string(char* line, char* pattern) {  
    regex_t regex;
    regmatch_t matches[1];
    int start, end;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "Не удалось скомпилировать регулярное выражение %s\n", pattern);
        return -1;
    }
    
    int found = 0;
    char* search_position = line;
    while (regexec(&regex, search_position, 1, matches, 0) == 0) {
        start = matches[0].rm_so;
        end = matches[0].rm_eo;
        printf("%.*s", start, search_position);
        printf("%s%.*s%s", RED, end - start, search_position + start, RESET);
        search_position += end;
        found++;
    }
    if (found) {
        printf("%s", search_position);
    }
    regfree(&regex);
    return 0;
}

int grep(int argc, char** argv) {
    if (argc > 1) {
        regex_t regex;
        int reti;
        for (int i = 1; i < argc; i++) {
            FILE* file;
            char* line = NULL;
            size_t len = 0;
            file = fopen(argv[i], "r");
            if (file == NULL) {
                fprintf(stderr, "Ошибка при открытии файла %s\n", argv[i]);
                return -1;
            }   
            while (getline(&line, &len, file) != -1) {
                if (process_string(line, argv[0]) == -1)
                    return -1;
            }
            free(line);
            fclose(file);
        } 
    }
    else if (argc == 1) {
        char* line = NULL;
        size_t len = 0; 
        while (getline(&line, &len, stdin) != -1) {
            if (process_string(line, argv[0]) == -1)
                return -1;
        }
        free(line);
    }
    else {
        printf("Использование: grep [ПАРАМЕТР]... ШАБЛОН [ФАЙЛ]...\n");
        return -1;
    }
    return 0;
}
