#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "elf.h"
#include <ncurses.h>


extern Elf64_Ehdr *elf;
extern Elf64_Phdr *phdr;
extern Elf64_Shdr *shdr;
extern char *strtab;
extern char *symstrtable;
extern FILE *fp;

extern char *STRING_INFO[];
extern char *BIND_TYPES[];
extern char *VISIBILITY[];


bool is_valid_elf();

char *get_symbol_name(const char *str_tab);

void create_program_header();         // Initializes contents for the program header if executable or shared file
void create_string_table();
Elf64_Shdr *create_section_header();  // Creates a section header (.bss, .table, .data, etc..)
void create_symbol_table();           // Creates the symbol table for each section
void create_section_header_table();   // Creates the section header table
void create_symbol_string_table();
void create_elf(char *file_name);
