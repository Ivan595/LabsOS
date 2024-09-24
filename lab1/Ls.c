#include "Ls.h"
#include "Vector.h"
#include "File.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ls(int argc, char** argv) {
    DIR* dir;
    struct dirent* entry;
    if (argc > 0) {
        for (int i = 0; i < argc; i++) {
            Vector* vector = create_vector();
            dir = opendir(argv[i]);
            if (dir == NULL) {
                fprintf(stderr, "my_ls: cannot access '%s': No such file or directory\n",
                        argv[i]);
                continue;
            }
            if (argc > 1) {
                printf("%s:\n", argv[i]);
            }
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_name[0] != '.') {
                    File* file = create_file(entry->d_name, argv[i]);
                    push_back(vector, file);
                }       
            }
            sort_vector(vector);
            for (int i = 0; i < vector->_size; i++) {
                print_name(vector->_data[i]);
                printf("  ");
            }
            if (i == argc - 1)
                printf("\n");
            else
                printf("\n\n");
            closedir(dir);
            free_vector(vector);
        }
    }
    else {
        Vector* vector = create_vector();
        dir = opendir(".");
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] != '.') {
                File* file = create_file(entry->d_name, NULL);
                push_back(vector, file);
            }
        }
        sort_vector(vector);
        for (int i = 0; i < vector->_size; i++) {
            print_name(vector->_data[i]);
            printf("  ");
        }
        printf("\n");
        closedir(dir);
        free_vector(vector);
    }
}

void ls_a(int argc, char** argv) {
    DIR* dir;
    struct dirent* entry;
    if (argc > 0) {
        for (int i = 0; i < argc; i++) {
            Vector* vector = create_vector();
            dir = opendir(argv[i]);
            if (dir == NULL) {
                fprintf(stderr, "my_ls: cannot access '%s': No such file or directory\n",
                        argv[i]);
                continue;
            }
            if (argc > 1) {
                printf("%s:\n", argv[i]);
            }
            while ((entry = readdir(dir)) != NULL) {
                File* file = create_file(entry->d_name, argv[i]);
                push_back(vector, file);
            }
            sort_vector(vector);
            for (int i = 0; i < vector->_size; i++) {
                print_name(vector->_data[i]);
                printf("  ");
            }
            if (i == argc - 1)
                printf("\n");
            else
                printf("\n\n");
            closedir(dir);
            free_vector(vector);
        }
    }
    else {
        Vector* vector = create_vector();
        dir = opendir(".");
        while ((entry = readdir(dir)) != NULL) {
            File* file = create_file(entry->d_name, NULL);
            push_back(vector, file);
        }
        sort_vector(vector);
        for (int i = 0; i < vector->_size; i++) {
            print_name(vector->_data[i]);
            printf("  ");
        }
        printf("\n");
        closedir(dir);
        free_vector(vector);
    }
}

void ls_l(int argc, char** argv) {
    DIR* dir;
    struct dirent* entry;
    if (argc > 0) {
        for (int i = 0; i < argc; i++) {
            Vector* vector = create_vector();
            dir = opendir(argv[i]);
            if (dir == NULL) {
                fprintf(stderr, "my_ls: cannot access '%s': No such file or directory\n",
                        argv[i]);
                continue;
            }
            if (argc > 1) {
                printf("%s:\n", argv[i]);
            }
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_name[0] != '.') {
                    File* file = create_file(entry->d_name, NULL);
                    push_back(vector, file);
                }  
            }
            sort_vector(vector);
            set_max_values(vector);
            printf("итого %ld\n", get_total_blocks_count(vector));
            for (int i = 0; i < vector->_size; i++) {
                print_info( vector->_data[i], vector->_max_nlink,
                            vector->_max_username, vector->_max_groupname,
                            vector->_max_size, vector->_max_date);
                printf("\n");
            }
            if (i != argc - 1)
                printf("\n");
            closedir(dir);
            free_vector(vector);
        }
    }
    else {
        Vector* vector = create_vector();
        dir = opendir(".");
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] != '.') {
                File* file = create_file(entry->d_name, NULL);
                push_back(vector, file);
            }
        }
        sort_vector(vector);
        set_max_values(vector);
        printf("итого %ld\n", get_total_blocks_count(vector));
        for (int i = 0; i < vector->_size; i++) {
            print_info( vector->_data[i], vector->_max_nlink,
                        vector->_max_username, vector->_max_groupname,
                        vector->_max_size, vector->_max_date);
            printf("\n");
        }
        closedir(dir);
        free_vector(vector);
    }
}

void ls_la(int argc, char** argv) {
    DIR* dir;
    struct dirent* entry;
    if (argc > 0) {
        for (int i = 0; i < argc; i++) {
            Vector* vector = create_vector();
            dir = opendir(argv[i]);
            if (dir == NULL) {
                fprintf(stderr, "my_ls: cannot access '%s': No such file or directory\n",
                        argv[i]);
                continue;
            }
            if (argc > 1) {
                printf("%s:\n", argv[i]);
            }
            while ((entry = readdir(dir)) != NULL) {
                File* file = create_file(entry->d_name, NULL);
                push_back(vector, file);
            }
            sort_vector(vector);
            set_max_values(vector);
            printf("итого %ld\n", get_total_blocks_count(vector));
            for (int i = 0; i < vector->_size; i++) {
                print_info( vector->_data[i], vector->_max_nlink,
                            vector->_max_username, vector->_max_groupname,
                            vector->_max_size, vector->_max_date);
                printf("\n");
            }
            if (i != argc - 1)
                printf("\n");
            closedir(dir);
            free_vector(vector);
        }
    }
    else {
        Vector* vector = create_vector();
        dir = opendir(".");
        while ((entry = readdir(dir)) != NULL) {
                File* file = create_file(entry->d_name, NULL);
                push_back(vector, file);
        }
        sort_vector(vector);
        set_max_values(vector);
        printf("итого %ld\n", get_total_blocks_count(vector));
        for (int i = 0; i < vector->_size; i++) {
            print_info( vector->_data[i], vector->_max_nlink,
                        vector->_max_username, vector->_max_groupname,
                        vector->_max_size, vector->_max_date);
            printf("\n");
        }
        closedir(dir);
        free_vector(vector);
    }
}
