#include "Vector.h"
#include "File.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Vector* create_vector() {
    Vector* vector = (Vector*)calloc(1, sizeof(Vector));
    vector->_data = (File**)calloc(INITIAL_CAPACITY, sizeof(File*));
    vector->_size = 0;
    vector->_capacity = INITIAL_CAPACITY;
    return vector;
}

void push_back(Vector* vector, File* file) {
    if (vector->_size >= vector->_capacity) {
        vector->_capacity *= 2;
        vector->_data = (File**)realloc(vector->_data, vector->_capacity * sizeof(File*));
    }
    vector->_data[vector->_size] = file;
    vector->_size++;
}

void free_vector(Vector* vector) {
    for (int i = 0; i < vector->_size; i++) {
        free(vector->_data[i]->_path);
        free(vector->_data[i]);
    }
    free(vector->_data);
    free(vector);
}




void set_max_values(Vector* vector) {
    vector->_max_nlink = 0;
    vector->_max_username = 0;
    vector->_max_groupname = 0;
    vector->_max_size = 0;
    vector->_max_date = 0;
    for (int i = 0; i < vector->_size; i++) {
        File* file = vector->_data[i];
        char* nlink = display_nlink(file->_type, file->_file_info, file->_link_info);
        char* username = display_username(file->_type, file->_file_info, file->_link_info);
        char* groupname = display_groupname(file->_type, file->_file_info, file->_link_info);
        char* size = display_size(file->_type, file->_file_info, file->_link_info);
        char* date = display_date(file->_type, file->_file_info, file->_link_info);
        int prev;
        prev = vector->_max_nlink;
        vector->_max_nlink = strlen(nlink) > prev ? strlen(nlink) : prev;
        prev = vector->_max_username;
        vector->_max_username = strlen(username) > prev ? strlen(username) : prev;  
        prev = vector->_max_groupname;
        vector->_max_groupname = strlen(groupname) > prev ? strlen(groupname) : prev;
        prev = vector->_max_size;
        vector->_max_size = strlen(size) > prev ? strlen(size) : prev;
        prev = vector->_max_date;
        vector->_max_date = strlen(date) > prev ? strlen(date) : prev;
        free(nlink);
        free(username);
        free(groupname);
        free(size);
        free(date);
    }
}



void sort_vector(Vector* vector) {
    for (int i = 0; i < vector->_size - 1; i++) {    
        for (int j = 0; j < vector->_size - i - 1; j++) {
            if (strcasecmp(vector->_data[j]->_name, vector->_data[j + 1]->_name) > 0) {
                File* temp = vector->_data[j];
                vector->_data[j] = vector->_data[j + 1];
                vector->_data[j + 1] = temp;
            }
        }
    }
}

long get_total_blocks_count(Vector* vector) {
    long total = 0;
    for (int i = 0 ; i < vector->_size; i++) {
        File* file = vector->_data[i];
        total += get_blocks_count(file->_type, file->_file_info, file->_link_info);
    }
    return total;
}   

