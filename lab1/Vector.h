#include "File.h"
#define INITIAL_CAPACITY 2

typedef struct {
    File** _data;
    int _size;
    int _capacity;
    int _max_nlink;
    int _max_username;
    int _max_groupname;
    int _max_size;
    int _max_date;
} Vector;

Vector* create_vector();

void push_back(Vector* vector, File* file);
void free_vector(Vector* vector);
void sort_vector();
void set_max_values(Vector* vector);
long get_total_blocks_count(Vector* vector);
