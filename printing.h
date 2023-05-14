#include <stdio.h>
#include <stdlib.h>
#include "elf.h"
#include <ncurses.h>
#include "./HUD/virtual.h"




extern Elf64_Ehdr *elf;             // elf header of executable file
extern char *symstrtable;           // symbol string table of executable
extern Elf64_Sym *symtab;           // symbol table
extern char *STRING_INFO[10];       // maps integer to string representation
extern char *BIND_TYPES[3];         // maps integer to bind types
extern char *VISIBILITY[4];         // maps integer to visibility

void print_ident   (unsigned char *e_ident);
char *print_type   (uint16_t type); 
char *print_machine(uint16_t machine);
void display_elf_header(); // displays contents of elf  header
char *deal_with_NDX(uint32_t index);
void deal_with_scroll(int num_entries, Elf64_Sym *symtab);
void deal_with_error();
void print_virtuals(WINDOW *win, char *pid);