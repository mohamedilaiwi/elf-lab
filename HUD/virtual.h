#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAX_BUFFER_SIZE 256     // BUFFER to store command and pid of running process

char *get_pid(char *file);      // returns the process given the file name
void get_line(char *pid, char *section, char **maps_line);
void get_virtual_address(char *pid, char *section, long *starting_address, long *ending_address);    // returns the virtual address range of the PID inside the heap given its process ID
void get_stack_ranges(char *pid, char **virtual_addresses);
void get_vvar_range(char *pid, char **virtual_addresses);
void get_vdso_range(char *pid, char **virtual_addresses);
void get_shared_libraries_range(char *pid, char **virtual_addresses);
void get_elf_sections_range(char *pid, char **virtual_addresses);
