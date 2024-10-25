#include "Archiver.h"
#include "File.h"
#include "Vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#define TEMP_ARCHIVE_NAME "temp"

void help() {
    FILE* file;
    char* line = NULL;
    size_t len = 0;
    file = fopen("help.txt", "r");
    while (getline(&line, &len, file) != -1) {
        printf("%s", line);
    }
    free(line);
    fclose(file);
}


int file_exists(int fd, char* name) {

    int file_name_length;
    
    while (read(fd, &file_name_length, sizeof(int)) > 0) {
    
        char* file_name = calloc(file_name_length, sizeof(char));
        read(fd, file_name, file_name_length);
        if (strcmp(file_name, name) == 0) {
            free(file_name);
            return 1;
        }
        time_t file_atime;
        read(fd, &file_atime, sizeof(time_t));
        time_t file_mtime;
        read(fd, &file_mtime, sizeof(time_t));
        mode_t file_mode;
        read(fd, &file_mode, sizeof(mode_t));
        uid_t file_uid;
        read(fd, &file_uid, sizeof(uid_t));
        gid_t file_gid;
        read(fd, &file_gid, sizeof(gid_t));
        off_t file_size;
        read(fd, &file_size, sizeof(off_t));
        int file_blocks_count;
        read(fd, &file_blocks_count, sizeof(int));
        
        void* file_content = calloc(file_size, sizeof(void));
        read(fd, file_content, file_size);

        free(file_content);
        free(file_name);
    }
    
    return 0;
}

int extract(char* archive_name, char* name) {
    
    int fd = open(archive_name, O_RDONLY);
    if (fd == -1) {
        printf("Архива не существует\n");
        return 1;
    }

    if (!file_exists(fd, name)) {
        printf("Нет такого файла в архиве\n");
        close(fd);
        return 1;
    }
    lseek(fd, 0, SEEK_SET);
    
    int temp_fd = open(TEMP_ARCHIVE_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    int file_name_length;
    
    while (read(fd, &file_name_length, sizeof(int)) > 0) {
    
        char* file_name = calloc(file_name_length, sizeof(char));
        read(fd, file_name, file_name_length);
        time_t file_atime;
        read(fd, &file_atime, sizeof(time_t));
        time_t file_mtime;
        read(fd, &file_mtime, sizeof(time_t));
        mode_t file_mode;
        read(fd, &file_mode, sizeof(mode_t));
        uid_t file_uid;
        read(fd, &file_uid, sizeof(uid_t));
        gid_t file_gid;
        read(fd, &file_gid, sizeof(gid_t));
        off_t file_size;
        read(fd, &file_size, sizeof(off_t));
        int file_blocks_count;
        read(fd, &file_blocks_count, sizeof(int));
        
        void* file_content = calloc(file_size, sizeof(void));
        read(fd, file_content, file_size);



        if (strcmp(file_name, name) != 0) {
            write(temp_fd, &file_name_length, sizeof(int));
            write(temp_fd, file_name, file_name_length);
            write(temp_fd, &file_atime, sizeof(time_t));
            write(temp_fd, &file_mtime, sizeof(time_t));
            write(temp_fd, &file_mode, sizeof(mode_t));
            write(temp_fd, &file_uid, sizeof(uid_t));
            write(temp_fd, &file_gid, sizeof(gid_t));
            write(temp_fd, &file_size, sizeof(off_t));
            write(temp_fd, &file_blocks_count, sizeof(int));
            write(temp_fd, file_content, file_size);
        }   
        else {
                    
            struct stat file_info;
            if (stat(name, &file_info) != 0) {
                int new_fd = open(name, O_WRONLY | O_CREAT, S_IWUSR);    
                write(new_fd, file_content, file_size);
                struct timespec times[2] = {
                    {file_atime, 0},
                    {file_mtime, 0}  
                }; 
                futimens(new_fd, times);
                close(new_fd);
                chmod(name, get_current_mode(file_mode));
                chown(name, file_uid, file_gid);
            }  
        }
        

        free(file_content);
        free(file_name);
    }
    
    close(fd);
    close(temp_fd);
    
    struct stat file_info;
    stat(archive_name, &file_info);
    chown(TEMP_ARCHIVE_NAME, file_info.st_uid, file_info.st_gid);

    remove(archive_name);
    rename(TEMP_ARCHIVE_NAME, archive_name);
    
    return 0;
    
}

void archive_stat(char* archive_name) {
    int fd = open(archive_name, O_RDONLY);
    if (fd == -1) {
        printf("Архива не существует\n");
        return;
    }

    int file_name_length;
    Vector* vector = create_vector();
    
    while (read(fd, &file_name_length, sizeof(int)) > 0) {
    
        char* file_name = calloc(file_name_length, sizeof(char));
        read(fd, file_name, file_name_length);
        time_t file_atime;
        read(fd, &file_atime, sizeof(time_t));
        time_t file_mtime;
        read(fd, &file_mtime, sizeof(time_t));
        mode_t file_mode;
        read(fd, &file_mode, sizeof(mode_t));
        uid_t file_uid;
        read(fd, &file_uid, sizeof(uid_t));
        gid_t file_gid;
        read(fd, &file_gid, sizeof(gid_t));
        off_t file_size;
        read(fd, &file_size, sizeof(off_t));
        int file_blocks_count;
        read(fd, &file_blocks_count, sizeof(int));
        
        void* file_content = calloc(file_size, sizeof(void));
        read(fd, file_content, file_size);

        File* file = create_file(file_name, file_atime, file_mtime, file_mode,
                                 file_uid, file_gid, file_size, file_blocks_count);
        push_back(vector, file);
        

        free(file_content);
        free(file_name);
    }

    sort_vector(vector);
    set_max_values(vector);
    printf("итого %d\n", get_total_blocks_count(vector));
    for (int i = 0; i < vector->_size; i++) {
        print_info(vector->_data[i], vector->_max_username,
                   vector->_max_groupname, vector->_max_size,
                   vector->_max_date);
        printf("\n");
    }
    free_vector(vector);
    close(fd);
}

int input(char* archive_name, char* file_name) {
    
    struct stat file_info;
    if (stat(file_name, &file_info) != 0) {
        printf("Нет такого файла\n");
        return 1;
    }
      
    int fd = open(archive_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    
    if (file_exists(fd, file_name)) {
        close(fd);
        return 0;
    }
    


    //get data
    int file_name_length = strlen(file_name) + 1;
    time_t file_atime = file_info.st_atime;
    time_t file_mtime = file_info.st_mtime;
    mode_t file_mode = file_info.st_mode;
    uid_t file_uid = file_info.st_uid;
    gid_t file_gid = file_info.st_gid;
    off_t file_size = file_info.st_size;
    int file_blocks_count = (long)file_info.st_blocks / 2;
    void* file_content = calloc(file_size, sizeof(void));
    int input_fd = open(file_name, O_RDONLY);
    read(input_fd, file_content, file_size);

    //write data
    write(fd, &file_name_length, sizeof(int));
    write(fd, file_name, file_name_length);
    write(fd, &file_atime, sizeof(time_t));
    write(fd, &file_mtime, sizeof(time_t));
    write(fd, &file_mode, sizeof(mode_t));
    write(fd, &file_uid, sizeof(uid_t));
    write(fd, &file_gid, sizeof(gid_t));
    write(fd, &file_size, sizeof(off_t));
    write(fd, &file_blocks_count, sizeof(int));
    write(fd, file_content, file_size);

    free(file_content);
    close(fd);
    close(input_fd);
    return 0;
}


