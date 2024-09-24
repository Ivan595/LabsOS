#ifndef FILE_H
#define FILE_H

#include <sys/stat.h>

typedef enum
{
    REGULAR,
    DIRECTORY,
    SYMBOLIC_LINK,
    BLOCK_DEVICE,
    CHARACTER_DEVICE,
    SOCKET,
    FIFO
} File_type;

typedef struct
{
	File_type _type;
    char* _path;
    char* _name; //можно передавать только название без выделения памяти
    struct stat _file_info, _link_info;
} File;

File* create_file(char* name, char* directory);
void print_name(File* file);
void print_info(File* file, int max_nlink, int max_username, int max_groupname, 
                int max_size, int max_date);

char* display_nlink(File_type type, struct stat file_info, struct stat link_info);
char* display_username(File_type type, struct stat file_info, struct stat link_info);
char* display_groupname(File_type type, struct stat file_info, struct stat link_info);
char* display_size(File_type type, struct stat file_info, struct stat link_info);
char* display_date(File_type type, struct stat file_info, struct stat link_info);
long get_blocks_count(File_type type, struct stat file_info, struct stat link_info);

#endif
