#include "File.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>

#define RESET "\033[0m"
#define BLUE "\033[34;1m"
#define YELLOW "\033[33;1m" 
#define ORANGE "\033[33m" 
#define CYAN "\033[36;1m"
#define MAGENTA "\033[35;1m"
#define GREEN "\033[32;1m"
#define DATE_MAX 80
#define PERMISSIONS_LEN 10

File_type get_type(mode_t file_mode, mode_t link_mode) {
    if ((link_mode & S_IFMT) == S_IFLNK)
        return SYMBOLIC_LINK;
    if ((file_mode & S_IFMT) == S_IFDIR)
        return DIRECTORY;
    if ((file_mode & S_IFMT) == S_IFBLK)
        return BLOCK_DEVICE;
    if ((file_mode & S_IFMT) == S_IFCHR)
        return CHARACTER_DEVICE;
    if ((file_mode & S_IFMT) == S_IFIFO)
        return FIFO;
    if ((file_mode & S_IFMT) == S_IFSOCK)
        return SOCKET;
    return REGULAR;    
}

File* create_file(char* name, char* directory) {
    File* file = (File*)calloc(1, sizeof(File));
    file->_name = name;
    if (directory == NULL) {
        //+1 нуль терминатор
        file->_path = (char*)calloc(strlen(name) + 1, sizeof(char));
        strcpy(file->_path, name);
    }
    else {
        //+2 один нуль терминатор и один бэкслеш между ними
        file->_path = (char*)calloc(strlen(name) + strlen(directory) + 2, sizeof(char));
        if (directory[strlen(directory) - 1] == '/') {
            strcpy(file->_path, directory);
            strcat(file->_path, name);
        }
        else {
            strcpy(file->_path, directory);
            strcat(file->_path, "/");
            strcat(file->_path, name);
        }
    }   

    stat(file->_path, &file->_file_info);
    lstat(file->_path, &file->_link_info);

    file->_type = get_type(file->_file_info.st_mode, file->_link_info.st_mode);
    return file;    
}

void printName(char* filename, File_type type, struct stat file_info) {
    if (type == DIRECTORY) {  
        printf("%s%s%s", BLUE, filename, RESET);
        return;
    }
    if (type == BLOCK_DEVICE || type == CHARACTER_DEVICE) {  
        printf("%s%s%s", YELLOW, filename, RESET);
        return;
    }
    if (type == FIFO) {  
        printf("%s%s%s", ORANGE, filename, RESET);
        return;
    }
    if (type == SYMBOLIC_LINK) {  
        printf("%s%s%s", CYAN, filename, RESET);
        return;
    }
    if (type == SOCKET) {  
        printf("%s%s%s", MAGENTA, filename, RESET);
        return;
    }

   if ((file_info.st_mode & S_IXUSR) ||
       (file_info.st_mode & S_IXGRP) ||
       (file_info.st_mode & S_IXOTH)) {
        printf("%s%s%s", GREEN, filename, RESET);
        return;
    }

    printf("%s", filename);
}

void print_name(File* file) {
    printName(file->_name, file->_type, file->_file_info);
}

void display_file_type(File_type type) {
    switch (type)
    {
        case BLOCK_DEVICE:      putchar('b'); break;
        case CHARACTER_DEVICE:  putchar('c'); break;
        case DIRECTORY:         putchar('d'); break;
        case FIFO:              putchar('p'); break;
        case SYMBOLIC_LINK:     putchar('l'); break;
        case SOCKET:            putchar('s'); break;
        case REGULAR:           putchar('-'); break;
    }   
}

void fill_permissions(char* permissions, mode_t mode) {
    permissions[0] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (mode & S_IXOTH) ? 'x' : '-';
    if (mode & S_ISUID)
        permissions[2] = 's';
    if (mode & S_ISGID)
        permissions[5] = 's';
    if (mode & S_ISVTX)
        permissions[8] = 't';
    permissions[9] = '\0';
}

void display_permissions(File_type type, mode_t file_mode, mode_t link_mode) {
    char* permissions = (char*)calloc(PERMISSIONS_LEN, sizeof(char));
    if (type == SYMBOLIC_LINK) {
        fill_permissions(permissions, link_mode);
    }
    else {
        fill_permissions(permissions, file_mode);
    }
    printf("%s", permissions);
    free(permissions);
}

char* display_nlink(File_type type, struct stat file_info, struct stat link_info) {
    char* rv = (char*)calloc(50, sizeof(char));
    if (type == SYMBOLIC_LINK) {
        sprintf(rv, "%d", link_info.st_nlink);
        return rv;
    }
    else {
        sprintf(rv, "%d", file_info.st_nlink);
        return rv;
    }   
}

char* display_username(File_type type, struct stat file_info, struct stat link_info) {
    char* rv = (char*)calloc(50, sizeof(char));
    struct passwd* ptr;
    if (type == SYMBOLIC_LINK) {
        if ((ptr = getpwuid(link_info.st_uid)) == NULL) {
            sprintf(rv, "%u", (unsigned int)link_info.st_uid);
        }
        else {
            sprintf(rv, "%s", ptr->pw_name);
        }
        return rv;
    }
    else {
        if ((ptr = getpwuid(file_info.st_uid)) == NULL) {
            sprintf(rv, "%u", (unsigned int)file_info.st_uid);
        }
        else {
            sprintf(rv, "%s", ptr->pw_name);
        }
        return rv;
    }
}

long get_blocks_count(File_type type, struct stat file_info, struct stat link_info) {
    if (type == SYMBOLIC_LINK) {
        return (long)link_info.st_blocks / 2;
    }
    else {
        return (long)file_info.st_blocks / 2;
    }
}

char* display_groupname(File_type type, struct stat file_info, struct stat link_info) {
    char* rv = (char*)calloc(50, sizeof(char));
    struct group* ptr;
    if (type == SYMBOLIC_LINK) {
        if ((ptr = getgrgid(link_info.st_gid)) == NULL) {
            sprintf(rv, "%d", link_info.st_gid);
        }
        else {
            sprintf(rv, "%s", ptr->gr_name);
        }
        return rv;
    }
    else {
        if ((ptr = getgrgid(file_info.st_gid)) == NULL) {
            sprintf(rv, "%d", file_info.st_gid);
        }
        else {
            sprintf(rv, "%s", ptr->gr_name);
        }
        return rv;
    }
}

char* display_size(File_type type, struct stat file_info, struct stat link_info) {
    char* rv = (char*)calloc(50, sizeof(char));
    //this code works on my machine
    /*if (type == BLOCK_DEVICE || type == CHARACTER_DEVICE) {
        sprintf(rv, "%d, %d", major(file_info.st_rdev), minor(file_info.st_rdev));
        return rv;
    }
    else {
        if (type == SYMBOLIC_LINK) {
            sprintf(rv, "%ld", link_info.st_size);
            return rv;
        }
        else {
            sprintf(rv, "%ld", file_info.st_size);
            return rv;
        }
    }*/

    if (type == SYMBOLIC_LINK) {
        sprintf(rv, "%ld", link_info.st_size);
        return rv;
    }
    else {
        sprintf(rv, "%ld", file_info.st_size);
        return rv;
    }


}

char* display_date(File_type type, struct stat file_info, struct stat link_info) {
    char* rv = (char*)calloc(50, sizeof(char));
    setlocale(LC_TIME, "ru_RU.UTF-8");
    if (type == SYMBOLIC_LINK) {
        struct tm* timeinfo = localtime(&link_info.st_mtime);
        strftime(rv, sizeof(rv), "%b %d %H:%M", timeinfo);
    }
    else {
        struct tm* timeinfo = localtime(&file_info.st_mtime);
        strftime(rv, sizeof(rv), "%b %d %H:%M", timeinfo);
    }
    return rv;
}

void display_filename(File* file)
{
    printf(" ");
    if (file->_type == SYMBOLIC_LINK) {
        char target_path[PATH_MAX];
        ssize_t len = readlink(file->_path, target_path, sizeof(target_path) - 1);
        target_path[len] = '\0';
        File_type type = get_type(file->_file_info.st_mode, file->_file_info.st_mode);
        
        print_name(file);
        printf(" -> ");
        printName(target_path, type, file->_file_info);
    }
    else {
        print_name(file);
    }
}

void print_info(File* file, int max_nlink, int max_username, int max_groupname, 
                int max_size, int max_date) {
    char* nlink = display_nlink(file->_type, file->_file_info, file->_link_info);
    char* username = display_username(file->_type, file->_file_info, file->_link_info);
    char* groupname = display_groupname(file->_type, file->_file_info, file->_link_info);
    char* size = display_size(file->_type, file->_file_info, file->_link_info);
    char* date = display_date(file->_type, file->_file_info, file->_link_info);
    display_file_type(file->_type);
    display_permissions(file->_type, file->_file_info.st_mode, file->_link_info.st_mode);
    printf("%*s", max_nlink + 1, nlink);
    printf(" %-*s", max_username, username);
    printf(" %-*s", max_groupname, groupname);
    printf("%*s", max_size + 1, size);
    printf("%*s", max_date + 1, date);
    display_filename(file);
    free(nlink);
    free(username);
    free(groupname);
    free(size);
    free(date);
}




