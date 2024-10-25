#ifndef ARCHIVER_H
#define ARCHIVER_H

void help();
int input(char* archive_name, char* file_name);
void archive_stat(char* archive_name);
int extract(char* archive_name, char* name);

#endif
