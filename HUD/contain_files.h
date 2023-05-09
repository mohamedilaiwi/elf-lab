#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>




typedef struct _DIRECTORY {
    struct dirent **files;
    int NUM_FILES;
} DIRECTORY;


int set_num_files               (char *file);      	           // defines the number of files in this directory
void get_files                  (DIRECTORY *directory, char *file);   // creates pointer-to-pointer of dirent files
bool is_dir                     (char *file_name);                      // returns true if file is directory
int change_color                (bool is_directory);
void free_array                 (struct dirent **files, int NUM_FILES);
struct dirent **initialize_array();