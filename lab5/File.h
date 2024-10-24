#ifndef FILE_H
#define FILE_H

#include <sys/types.h>

typedef struct
{
    char* _name;
    time_t _atime;
    time_t _mtime;
    mode_t _mode;
    uid_t _uid;
    gid_t _gid;
    off_t _size; 
    int _blocks_count;       
} File;


File* create_file(char* name, time_t atime, time_t mtime, mode_t mode,
                  uid_t uid, gid_t gid, off_t size, int blocks_count);

void free_file(File* file);

void print_info(File* file, int max_username, int max_groupname, 
                int max_size, int max_date);

char* get_username(uid_t uid);
char* get_groupname(gid_t gid);
char* get_size(off_t size);
char* get_date(time_t mtime);
int get_current_mode(mode_t mode);

#endif
