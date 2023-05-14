/*
*   Deals with retrieving information at address on machine.
*   Takes the PID of the process running, finds it in memory, and manually retrieves/writes at location.
*       Does this for each section of elf header (text, data, ...)
*/
#include "virtual.h"



char *get_pid(char *file) {
    FILE *fp;
    char command[MAX_BUFFER_SIZE];
    char *pid = malloc(MAX_BUFFER_SIZE);

    // constructing the command to search for process by name
    snprintf(command, sizeof(command), "pgrep test");

    // Open command for reading
    if ((fp = popen(command, "r")) == NULL) {
        fprintf(stderr, "Could not open file. Exiting..\n");
        exit(0);
    }

    // Reads the PID from command output
    if (fgets(pid, sizeof(pid), fp) != NULL) {
        // remove trailing newline, if present
        pid[strcspn(pid, "\n")] = '\0';
    } else {
        printf("PID could not be found.\n");
        return NULL;
    }
    return pid;
}



/*
*   get_line: gets range for given substring
*       Pre: PID is valid and currently running
*       Post: None
*/
void get_line(char *pid, char *section, char **maps_line) {
    FILE *fp;
    char pid_path[20];
    char line[256];

    snprintf(pid_path, 20, "/proc/%s/maps", pid);
    if ((fp = fopen(pid_path, "rb")) == NULL) {
        fprintf(stderr, "Could not open from file. Failed..\n");
        exit(1);
    }

    // Read and print the contents line by line
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, section) != NULL)
            break;
    }
    fclose(fp);
    *maps_line = line;
}

/*
*   get_virtual_address: Gets the range of the virtual address for a given running process.
*       Pre: None
*       Post: Returns the virtual addresses where:
*               start: heap_line[0]
*               end:   heap_line[1]
*/
void get_virtual_address(char *pid, char *section, long *starting_address, long *ending_address) {
    char *line;
    get_line(pid, section, &line);
    char *temp_addr = malloc(12);

    if (line == NULL) {
        *starting_address = -1;
        *ending_address   = -1;
    } 
    else {
        // char *virtual_addresses = strdup(strtok(line, " "));
        strncpy(temp_addr, line, 12);

        // char *token = strtok(virtual_addresses, "-");
        if (temp_addr != NULL) {
            *starting_address = strtol(temp_addr, NULL, 16);
            strncpy(temp_addr, line + 13, 12);
            *ending_address   = strtol(temp_addr, NULL, 16);
        }
    }
    free(temp_addr);
}


/*
*   Deals with returning the range for the STACK
*       Pre: Valid PID and currently running.
*       Post: None
*/
void get_stack_ranges(char *pid, char **virtual_addresses) {
    char *line;
    get_line(pid, "[stack]", &line);
}


/*
*   Deals with getting the virtual address range for VVAR
*       Pre: Valid PID and currently running.
*       Post: None
*/
void get_vvar_range(char *pid, char **virtual_addresses) {

}


/*
*   Deals with getting the vdso range
*       Pre: Valid PID and currently running.
*       Post: None
*/
void get_vdso_range(char *pid, char **virtual_addresses) {

}


/*
*   Deals with getting shared libraries range
*       Pre: Valid PID and currently running
*       Post: None
*/
void get_shared_libraries_range(char *pid, char **virtual_addresses) {

}


/*
*   Deals with getting ELF Sections Range
*       Pre: Valid PID and currently running
*       Post: None
*   
*       Virtual_addresses[0]: .text
*       Virtual_addresses[1]: .data
*       ....
*/
void get_elf_sections_range(char *pid, char **virtual_addresses) {
    
}
