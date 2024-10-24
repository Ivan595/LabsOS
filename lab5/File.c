#include "File.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#define PERMISSIONS_LEN 10

File* create_file(char* name, time_t atime, time_t mtime, mode_t mode,
                  uid_t uid, gid_t gid, off_t size, int blocks_count) {
    File* file = (File*)calloc(1, sizeof(File));

    file->_name = (char*)calloc(strlen(name) + 1, sizeof(char));
    strcpy(file->_name, name);
    file->_atime = atime;
    file->_mtime = mtime;
    file->_mode = mode;
    file->_uid = uid;
    file->_gid = gid;
    file->_size = size;
    file->_blocks_count = blocks_count;
    return file;
}

void free_file(File* file) {
    free(file->_name);
    free(file);
}

char* get_username(uid_t uid) {
    char* rv = (char*)calloc(50, sizeof(char));
    struct passwd* ptr;
    
    if ((ptr = getpwuid(uid)) == NULL) {
        sprintf(rv, "%u", (unsigned int)uid);
    }
    else {
        sprintf(rv, "%s", ptr->pw_name);
    }
    return rv;
}

char* get_groupname(gid_t gid) {
    char* rv = (char*)calloc(50, sizeof(char));
    struct group* ptr;
    if ((ptr = getgrgid(gid)) == NULL) {
        sprintf(rv, "%d", gid);
    }
    else {
        sprintf(rv, "%s", ptr->gr_name);
    }
    return rv;
}

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

void display_permissions(mode_t mode) {
    char* permissions = (char*)calloc(PERMISSIONS_LEN, sizeof(char));
    fill_permissions(permissions, mode);
    printf("%s", permissions);
    free(permissions);
}

char* get_size(off_t size) {
    char* rv = (char*)calloc(50, sizeof(char));
    sprintf(rv, "%d", (int)size);
    return rv;
}

char* get_date(time_t mtime) {
    char* rv = (char*)calloc(50, sizeof(char));
    setlocale(LC_TIME, "ru_RU.UTF-8");
    struct tm* timeinfo = localtime(&mtime);
    strftime(rv, 50, "%b %d %H:%M", timeinfo);

    return rv;
}

void print_info(File* file, int max_username, int max_groupname, 
                int max_size, int max_date) {
    char* username = get_username(file->_uid);
    char* groupname = get_groupname(file->_gid);
    char* size = get_size(file->_size);
    char* date = get_date(file->_mtime);
    printf("-");
    display_permissions(file->_mode);
    printf(" %-*s", max_username, username);
    printf(" %-*s", max_groupname, groupname);
    printf("%*s", max_size + 1, size);
    printf("%*s", max_date + 1, date);
    printf(" %s", file->_name);
    free(username);
    free(groupname);
    free(size);
    free(date);
}
