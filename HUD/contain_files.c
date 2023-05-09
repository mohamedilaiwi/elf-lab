#include "contain_files.h"
#include "logging.h"

/*
 * Counts the number of files in this current directory 
 * and sets it equal to NUM_FILES: the global variable in contain_files.h
 *
 * Pre: None
 * Post: NUM_FILES is initialize value
 */
int get_num_files(char *file) {
	DIR *current_dir = opendir(file);
	int NUM_FILES = 0;

	while(readdir(current_dir))
		NUM_FILES++;
	closedir(current_dir);
	return NUM_FILES;
}


/*
*	Allocates memory to store NUM_FILES of struct dirent objects.
*	Pre: None
*   Post: An allocated 2D array of struct dirent
*/
struct dirent **initialize_array(int NUM_FILES) {
	struct dirent **all_files = malloc(sizeof(struct dirent *) * NUM_FILES);
	for (int i = 0; i < NUM_FILES; i++) 
		all_files[i] = malloc(sizeof(struct dirent));
	return all_files;
}



/*
 * Returns a pointer to an array of directory entries. 
 * Pre: None
 * Post: Returns pointer to an array of directory entries
 * Return: Returns pointe to array if NUM_FILES > 0, else returns NULL
 *	  calling function should check to see if this return is NOT NULL
 */
void get_files(DIRECTORY *directory, char *file) {
	DIR *current_dir = opendir(file);
	if (!current_dir) {
		return;
	}
	directory -> NUM_FILES = get_num_files(file);
	directory->files = initialize_array(directory->NUM_FILES);

	for (int i = 0; i < directory->NUM_FILES; i++) 
		(directory->files)[i] = readdir(current_dir);		
}

/* 
*	Frees the previously allocated file system.
*/
void free_array(struct dirent **files, int NUM_FILES) {
	for (int i = 0; i < NUM_FILES; i++) {
		free(files[i]);
	}
	free(files);
}

/*
*	Returns true if file name is a directory
*/
bool is_dir(char *file_name) {
   struct stat statbuf;
   if (stat(file_name, &statbuf) != 0)
       return false;
	return S_ISDIR(statbuf.st_mode);
}