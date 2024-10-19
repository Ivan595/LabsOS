#include "Chmod.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <regex.h>
#include <sys/types.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#define FIRST_MASK "^[ugoa]*[+=-]+([rwxXst]*|[ugo])$"
#define SECOND_MASK "^[+=-]+[0-7]*$"
#define THIRD_MASK "^[0-7]*$"
#define KNOCK 509
#define RESET_U 63
#define RESET_G 455
#define RESET_O 504
#define READ_MODE 4
#define WRITE_MODE 2
#define EXECUTE_MODE 1
#define IS_EXECUTABLE 73
#define UID 2048
#define GID 1024
#define VTX 512

int get_current_mode(mode_t mode) {
    int s = 0;
    s += pow(2, 11) * (bool)(mode & S_ISUID);
    s += pow(2, 10) * (bool)(mode & S_ISGID);
    s += pow(2, 9) * (bool)(mode & S_ISVTX);
    s += pow(2, 8) * (bool)(mode & S_IRUSR);
    s += pow(2, 7) * (bool)(mode & S_IWUSR);
    s += pow(2, 6) * (bool)(mode & S_IXUSR);
    s += pow(2, 5) * (bool)(mode & S_IRGRP);
    s += pow(2, 4) * (bool)(mode & S_IWGRP);
    s += pow(2, 3) * (bool)(mode & S_IXGRP);
    s += pow(2, 2) * (bool)(mode & S_IROTH);
    s += pow(2, 1) * (bool)(mode & S_IWOTH);
    s += pow(2, 0) * (bool)(mode & S_IXOTH);
    return s;
}

int get_u(mode_t mode) {
    int s = 0;
    s += pow(2, 8) * (bool)(mode & S_IRUSR);
    s += pow(2, 7) * (bool)(mode & S_IWUSR);
    s += pow(2, 6) * (bool)(mode & S_IXUSR);    
    return s;
}    

int get_g(mode_t mode) {
    int s = 0;
    s += pow(2, 5) * (bool)(mode & S_IRGRP);
    s += pow(2, 4) * (bool)(mode & S_IWGRP);
    s += pow(2, 3) * (bool)(mode & S_IXGRP);
    return s;
}    

int get_o(mode_t mode) {
    int s = 0;
    s += pow(2, 2) * (bool)(mode & S_IROTH);
    s += pow(2, 1) * (bool)(mode & S_IWOTH);
    s += pow(2, 0) * (bool)(mode & S_IXOTH);
    return s;
}

int matches_first_mask(char* str) {
    regex_t first_regex;
    if (regcomp(&first_regex, FIRST_MASK, REG_EXTENDED) == 0) {
        if (regexec(&first_regex, str, 0, NULL, 0) == 0) {
            return 0;
        }
        else {
            return -1;
        }
    }
    else {
        return -2;
    }
}

int matches_second_mask(char* str) {
    regex_t second_mask;
    if (regcomp(&second_mask, SECOND_MASK, REG_EXTENDED) == 0) {
        if (regexec(&second_mask, str, 0, NULL, 0) == 0) {
            return 0;
        }
        else {
            return -1;
        }
    }
    else {
        return -2;
    }
}

int matches_third_mask(char* str) {
    regex_t third_mask;
    if (regcomp(&third_mask, THIRD_MASK, REG_EXTENDED) == 0) {
        if (regexec(&third_mask, str, 0, NULL, 0) == 0) {
            return 0;
        }
        else {
            return -1;
        }
    }
    else {
        return -2;
    }
}


void plus_u(char* before, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int s = get_u(mode);
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        //except w unless others or all were explicity specified
        s = s | (s >> 3) | (s >> 6);
        chmod(file_name, (s & KNOCK) | get_current_mode(mode));
    }
    for (int n = 0; n < strlen(before); n++) {
        switch (before[n]) {
            case 'u':
                //u_plus_u();
                //nothing changes
                break;
            case 'g':
                //g_plus_u();
                current_mode = (s >> 3) | current_mode;
                chmod(file_name, current_mode);    
                break;
            case 'o':
                //o_plus_u();
                current_mode = (s >> 6) | current_mode;
                chmod(file_name, current_mode);
                break;
            case 'a':
                //a_plus_u();
                current_mode = (s | (s >> 3) | (s >> 6)) | current_mode;
                chmod(file_name, current_mode);
                break;
        }
    }
}
    
void minus_u(char* before, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int s = get_u(mode);
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        //except w unless others or all were explicity specified
        s = s | (s >> 3) | (s >> 6);
        chmod(file_name, (current_mode ^ (s & KNOCK)) & current_mode);
    } 
    for (int n = 0; n < strlen(before); n++) {
        switch (before[n]) {
            case 'u':
                //u_minus_u();
                current_mode = (current_mode ^ s) & current_mode;
                chmod(file_name, current_mode);
                break;
            case 'g':
                //g_minus_u();
                current_mode = (current_mode ^ (s >> 3)) & current_mode;
                chmod(file_name, current_mode);
                break;
            case 'o':
                //o_minus_u();
                current_mode = (current_mode ^ (s >> 6)) & current_mode;
                chmod(file_name, current_mode);
                break;
            case 'a':
                //a_minus_u();
                current_mode = (current_mode ^ (s | (s >> 3) | (s >> 6))) & current_mode;
                chmod(file_name, current_mode);
                break;
        }
    }   
}

void equal_u(char* before, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int s = get_u(mode);
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        //except w unless others or all were explicity specified
        //knock out w-bit from others
        s = s | (s >> 3) | (s >> 6);
        chmod(file_name, (s & KNOCK));
    } 
    for (int n = 0; n < strlen(before); n++) {
        switch (before[n]) {
            case 'u':
                //u_equal_u();
                //nothing changes
                break;
            case 'g':
                //g_equal_u();
                current_mode = (s >> 3) | (current_mode & RESET_G);
                chmod(file_name, current_mode);
                break;
            case 'o':
                //o_equal_u();
                current_mode = (s >> 6) | (current_mode & RESET_O);
                chmod(file_name, current_mode);
                break;
            case 'a':
                //a_equal_u();
                current_mode = s | (s >> 3) | (s >> 6);
                chmod(file_name, current_mode);
                break;
        }
    }   
}

void plus_g(char* before, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int s = get_g(mode);
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        //except w unless others or all were explicity specified
        s = s | (s << 3) | (s >> 3);
        chmod(file_name, (s & KNOCK) | current_mode);
    }
    for (int n = 0; n < strlen(before); n++) {
        switch (before[n]) {
            case 'u':
                //u_plus_g();
                current_mode = (s << 3) | current_mode;
                chmod(file_name, current_mode);
                break;
            case 'g':
                //g_plus_g();
                //do nothing    
                break;
            case 'o':
                //o_plus_g();
                current_mode = (s >> 3) | current_mode;
                chmod(file_name, current_mode);
                break;
            case 'a':
                //a_plus_g();
                current_mode = (s | (s << 3) | (s >> 3)) | current_mode;
                chmod(file_name, current_mode);
                break;
        }
    }
}

void minus_g(char* before, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int s = get_g(mode);
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        //except w unless others or all were explicity specified
        s = s | (s << 3) | (s >> 3);
        chmod(file_name, (current_mode ^ (s & KNOCK)) & current_mode);
    } 
    for (int n = 0; n < strlen(before); n++) {
        switch (before[n]) {
            case 'u':
                //u_minus_g();
                current_mode = (current_mode ^ (s << 3)) & current_mode;
                chmod(file_name, current_mode);
                break;
            case 'g':
                //g_minus_g();
                current_mode = (current_mode ^ s) & current_mode;
                chmod(file_name, current_mode);
                break;
            case 'o':
                //o_minus_g();
                current_mode = (current_mode ^ (s >> 3)) & current_mode;
                chmod(file_name, current_mode);
                break;
            case 'a':
                //a_minus_g();
                current_mode = (current_mode ^ (s | (s << 3) | (s >> 3))) & current_mode;
                chmod(file_name, current_mode);
                break;
        }
    }   
}

void equal_g(char* before, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int s = get_g(mode);
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        //except w unless others or all were explicity specified
        //knock out w-bit from others
        s = s | (s << 3) | (s >> 3);
        chmod(file_name, (s & KNOCK));
    } 
    for (int n = 0; n < strlen(before); n++) {
        switch (before[n]) {
            case 'u':
                //u_equal_g();
                current_mode = (s << 3) | (current_mode & RESET_U);
                chmod(file_name, current_mode);
                break;
            case 'g':
                //g_equal_g();
                //do nothing
                break;
            case 'o':
                //o_equal_g();
                current_mode = (s >> 3) | (current_mode & RESET_O);
                chmod(file_name, current_mode);
                break;
            case 'a':
                //a_equal_g();
                current_mode = s | (s << 3) | (s >> 3);
                chmod(file_name, current_mode);
                break;
        }
    }   
}

void plus_o(char* before, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int s = get_o(mode);
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        //except w unless others or all were explicity specified
        s = s | (s << 3) | (s << 6);
        chmod(file_name, (s & KNOCK) | current_mode);
    }
    for (int n = 0; n < strlen(before); n++) {
        switch (before[n]) {
            case 'u':
                //u_plus_o();
                current_mode = (s << 6) | current_mode;
                chmod(file_name, current_mode);
                break;
            case 'g':
                //g_plus_o();
                current_mode = (s << 3) | current_mode;
                chmod(file_name, current_mode);   
                break;
            case 'o':
                //o_plus_o();
                //do nothing
                break;
            case 'a':
                //a_plus_o();
                current_mode = (s | (s << 3) | (s << 6)) | current_mode;
                chmod(file_name, current_mode);
                break;
        }
    }
}

void minus_o(char* before, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int s = get_o(mode);
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        //except w unless others or all were explicity specified
        s = s | (s << 3) | (s << 6);
        chmod(file_name, (current_mode ^ (s & KNOCK)) & current_mode);
    } 
    for (int n = 0; n < strlen(before); n++) {
        switch (before[n]) {
            case 'u':
                //u_minus_o();
                current_mode = (current_mode ^ (s << 6)) & current_mode;
                chmod(file_name, current_mode);
                break;
            case 'g':
                //g_minus_o();
                current_mode = (current_mode ^ (s << 3)) & current_mode;
                chmod(file_name, current_mode);
                break;
            case 'o':
                //o_minus_o();
                current_mode = (current_mode ^ s) & current_mode;
                chmod(file_name, current_mode);
                break;
            case 'a':
                //a_minus_o();
                current_mode = (current_mode ^ (s | (s << 3) | (s << 6))) & current_mode;
                chmod(file_name, current_mode);
                break;
        }
    }   
}

void equal_o(char* before, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int s = get_o(mode);
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        //except w unless others or all were explicity specified
        //knock out w-bit from others
        s = s | (s << 3) | (s << 6);
        chmod(file_name, (s & KNOCK));
    } 
    for (int n = 0; n < strlen(before); n++) {
        switch (before[n]) {
            case 'u':
                //u_equal_o();
                current_mode = (s << 6) | (current_mode & RESET_U);
                chmod(file_name, current_mode);
                break;
            case 'g':
                //g_equal_o();
                current_mode = (s << 3) | (current_mode & RESET_G);
                chmod(file_name, current_mode);
                break;
            case 'o':
                //o_equal_o();
                //do nothing
                break;
            case 'a':
                //a_equal_o();
                current_mode = s | (s << 3) | (s << 6);
                chmod(file_name, current_mode);
                break;
        }
    }   
}

void process_ugo(char* before, char* signs, char ch, char* file_name) {
    switch (ch) {
        case 'u': {
            for (int k = 0; k < strlen(signs); k++) {
                switch (signs[k]) {
                    case '+':
                        plus_u(before, file_name);
                        break;
                    case '-':
                        minus_u(before, file_name);
                        break;
                    case '=':
                        equal_u(before, file_name);
                        break;
                }
            }       
            break;
        }
        case 'g':
            for (int k = 0; k < strlen(signs); k++) {
                switch (signs[k]) {
                    case '+':
                        plus_g(before, file_name);
                        break;
                    case '-':
                        minus_g(before, file_name);
                        break;
                    case '=':
                        equal_g(before, file_name);    
                        break;
                }
            }
            break;
        case 'o':
            for (int k = 0; k < strlen(signs); k++) {
                switch (signs[k]) {
                    case '+':
                        plus_o(before, file_name);
                        break;
                    case '-':
                        minus_o(before, file_name);
                        break;
                    case '=':
                        equal_o(before, file_name);
                        break;
                }
            }
            break;
    }
}

void plus_rwx(char* before, char* after, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        if (strchr(after, 'r') != NULL) {
            current_mode = current_mode | (READ_MODE | (READ_MODE << 3) | (READ_MODE << 6));
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = current_mode | (WRITE_MODE | (WRITE_MODE << 3) | (WRITE_MODE << 6));
            current_mode = current_mode & KNOCK;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = current_mode | (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6));
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            //if directory or already executable
            if (((mode & S_IFMT) == S_IFDIR) || (current_mode & IS_EXECUTABLE)) {
                current_mode = current_mode | (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6));
                chmod(file_name, current_mode);
            }
        }
        if (strchr(after, 's') != NULL) {
            current_mode = current_mode | UID | GID;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            current_mode = current_mode | VTX;
            chmod(file_name, current_mode);
        }
    }
    if (strchr(before, 'u') != NULL) {

        if (strchr(after, 'r') != NULL) {
            current_mode = current_mode | (READ_MODE << 6);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode =  current_mode | (WRITE_MODE << 6);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = current_mode | (EXECUTE_MODE << 6);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            //if directory or already executable
            if (((mode & S_IFMT) == S_IFDIR) || (current_mode & IS_EXECUTABLE)) {
                current_mode = current_mode | (EXECUTE_MODE << 6);
                chmod(file_name, current_mode);
            }
        }
        if (strchr(after, 's') != NULL) {
            current_mode = current_mode | UID;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {   
            //do nothing            
        }
    }
    if (strchr(before, 'g') != NULL) {
    
        if (strchr(after, 'r') != NULL) {
            current_mode = current_mode | (READ_MODE << 3);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = current_mode | (WRITE_MODE << 3);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = current_mode | (EXECUTE_MODE << 3);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            //if directory or already executable
            if (((mode & S_IFMT) == S_IFDIR) || (current_mode & IS_EXECUTABLE)) {
                current_mode = current_mode | (EXECUTE_MODE << 3);
                chmod(file_name, current_mode);
            }
        }
        if (strchr(after, 's') != NULL) {
            current_mode = current_mode | GID;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            //do nothing
        }
    }
    if (strchr(before, 'o') != NULL) {

        if (strchr(after, 'r') != NULL) {
            current_mode = current_mode | READ_MODE;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = current_mode | WRITE_MODE;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = current_mode | EXECUTE_MODE;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            //if directory or already executable
            if (((mode & S_IFMT) == S_IFDIR) || (current_mode & IS_EXECUTABLE)) {
                current_mode = current_mode | EXECUTE_MODE;
                chmod(file_name, current_mode);
            }
        }
        if (strchr(after, 's') != NULL) {
           //do nothing 
        }
        if (strchr(after, 't') != NULL) {
            current_mode = current_mode | VTX;
            chmod(file_name, current_mode);
        }
    }
    if (strchr(before, 'a') != NULL) {
        if (strchr(after, 'r') != NULL) {
            current_mode = current_mode | (READ_MODE | (READ_MODE << 3) | (READ_MODE << 6));
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = current_mode | (WRITE_MODE | (WRITE_MODE << 3) | (WRITE_MODE << 6));
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = current_mode | (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6));
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            //if directory or already executable
            if (((mode & S_IFMT) == S_IFDIR) || (current_mode & IS_EXECUTABLE)) {
                current_mode = current_mode | (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6));
                chmod(file_name, current_mode);
            }
        }
        if (strchr(after, 's') != NULL) {
            current_mode = current_mode | UID | GID;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            current_mode = current_mode | VTX;
            chmod(file_name, current_mode);
        }
    }
}

void minus_rwx(char* before, char* after, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        if (strchr(after, 'r') != NULL) {
            current_mode = (current_mode ^ (READ_MODE | (READ_MODE << 3) | (READ_MODE << 6))) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = (current_mode ^ ((WRITE_MODE | (WRITE_MODE << 3) | (WRITE_MODE << 6)) & KNOCK)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = (current_mode ^ (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6))) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            current_mode = (current_mode ^ (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6))) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 's') != NULL) {
            current_mode = (current_mode ^ (UID | GID)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            current_mode = (current_mode ^ (VTX)) & current_mode;
            chmod(file_name, current_mode);
        }
    }
    if (strchr(before, 'u') != NULL) {
        if (strchr(after, 'r') != NULL) {
            current_mode = (current_mode ^ (READ_MODE << 6)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = (current_mode ^ (WRITE_MODE << 6)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = (current_mode ^ (EXECUTE_MODE << 6)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            current_mode = (current_mode ^ (EXECUTE_MODE << 6)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 's') != NULL) {
            current_mode = (current_mode ^ (UID)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            //do nothing
        }
    }
    if (strchr(before, 'g') != NULL) {
        if (strchr(after, 'r') != NULL) {
            current_mode = (current_mode ^ (READ_MODE << 3)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = (current_mode ^ (WRITE_MODE << 3)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = (current_mode ^ (EXECUTE_MODE << 3)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            current_mode = (current_mode ^ (EXECUTE_MODE << 3)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 's') != NULL) {
            current_mode = (current_mode ^ (GID)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            //do nothing
        }
    }
    if (strchr(before, 'o') != NULL) {
        if (strchr(after, 'r') != NULL) {
            current_mode = (current_mode ^ (READ_MODE)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = (current_mode ^ (WRITE_MODE)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = (current_mode ^ (EXECUTE_MODE)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            current_mode = (current_mode ^ (EXECUTE_MODE)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 's') != NULL) {
            //do nothing
        }
        if (strchr(after, 't') != NULL) {
            current_mode = (current_mode ^ (VTX)) & current_mode;
            chmod(file_name, current_mode);
        }
    }
    if (strchr(before, 'a') != NULL) {
        if (strchr(after, 'r') != NULL) {
            current_mode = (current_mode ^ (READ_MODE | (READ_MODE << 3) | (READ_MODE << 6))) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = (current_mode ^ (WRITE_MODE | (WRITE_MODE << 3) | (WRITE_MODE << 6))) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = (current_mode ^ (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6))) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            current_mode = (current_mode ^ (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6))) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 's') != NULL) {
            current_mode = (current_mode ^ (UID | GID)) & current_mode;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            current_mode = (current_mode ^ (VTX)) & current_mode;
            chmod(file_name, current_mode);
        }
    }
}

void equal_rwx(char* before, char* after, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int current_mode = get_current_mode(mode);
    if (strlen(before) == 0) {
        current_mode = 0;
        if (strchr(after, 'r') != NULL) {
            current_mode = current_mode | (READ_MODE | (READ_MODE << 3) | (READ_MODE << 6));
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = current_mode | ((WRITE_MODE | (WRITE_MODE << 3) | (WRITE_MODE << 6)) & KNOCK);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = current_mode | (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6));
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            //if directory or already executable
            if (((mode & S_IFMT) == S_IFDIR) || (current_mode & IS_EXECUTABLE)) {
                current_mode = current_mode | (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6));
                chmod(file_name, current_mode);
            }
        }
        if (strchr(after, 's') != NULL) {
            current_mode = current_mode | (UID | GID);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            current_mode = current_mode | (VTX);
            chmod(file_name, current_mode);
        }
    }
    if (strchr(before, 'u') != NULL) {
        current_mode = current_mode & RESET_U;
        if (strchr(after, 'r') != NULL) {
            current_mode = current_mode | (READ_MODE << 6);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = current_mode | (WRITE_MODE << 6);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = current_mode | (EXECUTE_MODE << 6);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            //if directory or already executable
            if (((mode & S_IFMT) == S_IFDIR) || (current_mode & IS_EXECUTABLE)) {
                current_mode = current_mode | (EXECUTE_MODE << 6);
                chmod(file_name, current_mode);
            }
        }
        if (strchr(after, 's') != NULL) {
            current_mode = current_mode | UID;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            //do nothing
        }
    }
    if (strchr(before, 'g') != NULL) {
        current_mode = current_mode & RESET_G;
        if (strchr(after, 'r') != NULL) {
            current_mode = current_mode | (READ_MODE << 3);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = current_mode | (WRITE_MODE << 3);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = current_mode | (EXECUTE_MODE << 3);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            //if directory or already executable
            if (((mode & S_IFMT) == S_IFDIR) || (current_mode & IS_EXECUTABLE)) {
                current_mode = current_mode | (EXECUTE_MODE << 3);
                chmod(file_name, current_mode);
            }
        }
        if (strchr(after, 's') != NULL) {
            current_mode = current_mode | GID;
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            //do nothing
        }
    }
    if (strchr(before, 'o') != NULL) {
        current_mode = current_mode & RESET_O;
        if (strchr(after, 'r') != NULL) {
            current_mode = current_mode | (READ_MODE);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = current_mode | (WRITE_MODE);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = current_mode | (EXECUTE_MODE);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            //if directory or already executable
            if (((mode & S_IFMT) == S_IFDIR) || (current_mode & IS_EXECUTABLE)) {
                current_mode = current_mode | (EXECUTE_MODE);
                chmod(file_name, current_mode);
            }
        }
        if (strchr(after, 's') != NULL) {
            //do nothing
        }
        if (strchr(after, 't') != NULL) {   
            current_mode = current_mode | VTX;
            chmod(file_name, current_mode);
        }
    }
    if (strchr(before, 'a') != NULL) {
        current_mode = 0;
        if (strchr(after, 'r') != NULL) {
            current_mode = current_mode | (READ_MODE | (READ_MODE << 3) | (READ_MODE << 6));
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'w') != NULL) {
            current_mode = current_mode | (WRITE_MODE | (WRITE_MODE << 3) | (WRITE_MODE << 6));
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'x') != NULL) {
            current_mode = current_mode | (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6));
            chmod(file_name, current_mode);
        }
        if (strchr(after, 'X') != NULL) {
            //if directory or already executable
            if (((mode & S_IFMT) == S_IFDIR) || (current_mode & IS_EXECUTABLE)) {
                current_mode = current_mode | (EXECUTE_MODE | (EXECUTE_MODE << 3) | (EXECUTE_MODE << 6));
                chmod(file_name, current_mode);
            }
        }
        if (strchr(after, 's') != NULL) {
            current_mode = current_mode | (UID | GID);
            chmod(file_name, current_mode);
        }
        if (strchr(after, 't') != NULL) {
            current_mode = current_mode | (VTX);
            chmod(file_name, current_mode);
        }
    }
}

void process_rwx(char* before, char* signs, char* after, char* file_name) {

    for (int k = 0; k < strlen(signs); k++) {
        switch (signs[k]) {
            case '+':
                plus_rwx(before, after, file_name);
                break;
            case '-':
                minus_rwx(before, after, file_name);
                break;
            case '=':
                equal_rwx(before, after, file_name);
                break;
        }
    }       
}

int process_first_mask(char* str, int argc, char** argv) {
    for (int i = 2; i < argc; i++) {
        char* file_name = argv[i];

        regex_t regex;
        regcomp(&regex, "[+=-]+", REG_EXTENDED);
        regmatch_t pmatch[1];
        regexec(&regex, str, 1, pmatch, 0);
        int start = pmatch[0].rm_so;
        int end = pmatch[0].rm_eo; 
        char* before = (char*)calloc(start + 1, sizeof(char));
        char* signs = (char*)calloc(end - start + 1, sizeof(char));
        char* after = (char*)calloc(strlen(str) - end + 1, sizeof(char));
        //part before signs
        sprintf(before, "%.*s", start, str);
        //signs
        sprintf(signs, "%.*s", end - start, str + start);
        //part after signs
        sprintf(after, "%.*s", strlen(str) - end, str + end);
        
        //after = [ugo]
        if (strchr(after, 'u') || strchr(after, 'g') || strchr(after, 'o')) {
            process_ugo(before, signs, after[0], file_name);
        }
        //after = [rwxXst]*
        else {
            process_rwx(before, signs, after, file_name);
        }
        

        free(before);
        free(signs);
        free(after);
    }
    return 0;
}

int plus_number(char* after, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int current_mode = get_current_mode(mode);
    
    long number = strtol(after, NULL, 8);
    if (number > 4095) {
        return -1;
    }
    else {
        current_mode = current_mode | number;
        chmod(file_name, current_mode);
        return 0;
    }
}

int minus_number(char* after, char* file_name) {
    struct stat file_info;
    stat(file_name, &file_info);
    mode_t mode = file_info.st_mode;
    int current_mode = get_current_mode(mode);
    
    long number = strtol(after, NULL, 8);
    if (number > 4095) {
        return -1;
    }
    else {
        current_mode = (current_mode ^ number) & current_mode;
        chmod(file_name, current_mode);
        return 0;
    }
}

int equal_number(char* after, char* file_name) {
    long number = strtol(after, NULL, 8);
    if (number > 4095) {
        return -1;
    }
    else {
        chmod(file_name, number);
        return 0;
    }
}

int process_signs_numbers(char* signs, char* after, char* file_name) {
    for (int k = 0; k < strlen(signs); k++) {
        switch (signs[k]) {
            case '+':
                if (plus_number(after, file_name) == -1) {
                    return -1;
                }
                break;
            case '-':
                if (minus_number(after, file_name) == -1) {
                    return -1;
                }
                break;
            case '=':
                if (equal_number(after, file_name) == -1) {
                    return -1;
                }
                break;
        }
    }
    return 0;
}

int process_second_mask(char* str, int argc, char** argv) {
    for (int i = 2; i < argc; i++) {
        char* file_name = argv[i];        
        regex_t regex;
        regcomp(&regex, "[+=-]+", REG_EXTENDED);
        regmatch_t pmatch[1];
        regexec(&regex, str, 1, pmatch, 0);
        int start = pmatch[0].rm_so;
        int end = pmatch[0].rm_eo; 
        char* signs = (char*)calloc(end - start + 1, sizeof(char));
        char* after = (char*)calloc(strlen(str) - end + 1, sizeof(char));
        //signs
        sprintf(signs, "%.*s", end - start, str + start);
        //part after signs
        sprintf(after, "%.*s", strlen(str) - end, str + end);
        
        if (process_signs_numbers(signs, after, file_name) == -1) {
            printf("my_chmod: неверный режим: «%s»\n", str);
            return -1;
        }
        
        free(signs);
        free(after);
    }
    return 0;
}

int check_two_masks(char* str, int argc, char** argv) {
    int res1 = matches_first_mask(str);
    int res2 = matches_second_mask(str);

    if (res1 == -2 || res2 == -2) {
        printf("Ошибка компиляции регулярного выражения\n");
        return -2;
    }
    else if (res1 == 0) {
        return 0;
    }
    else if (res2 == 0) {
        return 0;
    }
    else {
        printf("my_chmod: неверный режим: «%s»\n", str);
        return -1;
    }
}

int process_two_masks(char* str, int argc, char** argv) {
    int res1 = matches_first_mask(str);
    int res2 = matches_second_mask(str);

    if (res1 == 0) {
        return process_first_mask(str, argc, argv);
    }
    if (res2 == 0) {
        return process_second_mask(str, argc, argv);
    }
}

int process_third_mask(char* str, int argc, char** argv) {
    for (int i = 2; i < argc; i++) {
        char* file_name = argv[i];

        long number = strtol(str, NULL, 8);
        if (number > 4095) {
            printf("my_chmod: неверный режим: «%s»\n", str);
            return -1;
        }
        else {
            chmod(file_name, number);
        }
        
    
    }
    return 0;
}

int process_three_masks(char* str, int argc, char** argv) {
    int res1 = matches_first_mask(str);
    int res2 = matches_second_mask(str);
    int res3 = matches_third_mask(str);

    if (res1 == -2 | res2 == -2 | res3 == -2) {
        printf("Ошибка компиляции регулярного выражения\n");
        return -2;
    }

    else if (res1 == 0) {
        return process_first_mask(str, argc, argv);
    }
    else if (res2 == 0) {
        return process_second_mask(str, argc, argv);
    }
    else if (res3 == 0) {
        return process_third_mask(str, argc, argv);
    }
    else {
        printf("my_chmod: неверный режим: «%s»\n", str);
        return -1;
    }

}

int my_chmod(int argc, char** argv) {
    char* pattern = argv[1];
    if (strchr(pattern, ',') != NULL) {
        int res;
        char* temp = (char*)calloc(strlen(pattern) + 1, sizeof(char));
        strcpy(temp, pattern);
        char sep[2] = ",";
        char* istr;
        istr = strtok(temp, sep);
        while (istr != NULL) {
            if ((res = check_two_masks(istr, argc, argv)) != 0) {
                return res;
            }
            istr = strtok(NULL, sep);
        }

        strcpy(temp, pattern);
        istr = strtok(temp, sep);
        while (istr != NULL) {
            if ((res = process_two_masks(istr, argc, argv)) != 0) {
                return res;
            }   
            istr = strtok(NULL, sep);
        }
        free(temp);
    }
    else {
        int res;
        if ((res = process_three_masks(pattern, argc, argv)) != 0) {
            return res;
        }
    }
    return 0;
}


