#include "printing.h"

/*
*   Prints class, data encoding, version, operating system
*   Precondition: Elf is not null
*/
void print_ident(unsigned char *e_ident) {
    wprintw(stdscr, "Class: \n");
    wprintw(stdscr, "\t\t%s\n", e_ident[EI_CLASS] == ELFCLASS32 ? "ARCH32" : "ARCH64");
    wprintw(stdscr, "\t\t%s\n", e_ident[EI_VERSION] == EV_NONE ? "Invalid Version" : "Current Version");
    wprintw(stdscr, "\t\t");
    switch(e_ident[EI_OSABI]) {
        case ELFOSABI_SYSV:
            wprintw(stdscr, "UNIX SYSTEM V ABI\n"); break;
        case ELFOSABI_LINUX:
            wprintw(stdscr, "Linux ABI\n"); break;
        case ELFOSABI_ARM:
            wprintw(stdscr, "ARM Architecture ABI\n"); break;
        default: wprintw(stdscr, "Could not find ABI\n"); break;
    }
}

/*
*   Prints type of file (executable, relocatable, shared, etc...)
*/
char *print_type(uint16_t type) {
    switch (type) {
        case ET_REL:  return "Relocatable File";
        case ET_EXEC: return "Executable File";
        case ET_DYN:  return "Shared object";
        case ET_CORE: return "Core file";
        default: break;
    }

    return "Unknown type";
}

/*
*   Prints the required architecture for individual file
*/
char *print_machine(uint16_t machine) {
    switch(machine) {
        case EM_M32:    return "AT&T WE 32100";
        case EM_SPARC:  return "Sun Microsystems SPARC";
        case EM_386:    return "Intel 80386";
        case EM_S390:   return "IBM S/390";
        case EM_X86_64: return "AMD x86-64";
        default: break;
    }

    return "Unknown machine";
}

void display_elf_header() {
    print_ident(elf->e_ident);
    wprintw(stdscr, "Type:                                           %s\n", print_type(elf->e_type));
    wprintw(stdscr, "Machine:                                        %s\n", print_machine(elf->e_machine));
    wprintw(stdscr, "Entry Point:                                    %li\n", elf->e_entry);
    wprintw(stdscr, "Program Header Offset:                          %li\n", elf->e_phoff);
    wprintw(stdscr, "Section Header Offset:                          %li\n", elf->e_shoff);
    wprintw(stdscr, "Processor Flags:                                %i\n",  elf->e_flags);
    wprintw(stdscr, "Elf Header Size:                                %i\n",  elf->e_ehsize);
    wprintw(stdscr, "Size of each entry in Program Header:           %i\n",  elf->e_phentsize);
    wprintw(stdscr, "Number of entries in Program Header:            %i\n",  elf->e_phnum);
    wprintw(stdscr, "Size of Section Header:                         %i\n",  elf->e_shentsize);
    wprintw(stdscr, "Number of entries in the Section Header:        %i\n",  elf->e_shnum);
    wprintw(stdscr, "Section Header Table Index of entry:            %i\n",  elf->e_shstrndx);
}


/*
*	deal_with_scroll: Gives user the functionality of being able to scroll through the outputs.
*	Pre: None
*	Post: Prints to stdscr contents of selected choice (display_information, see_section, etc...)
*/
void deal_with_scroll(int num_entries, Elf64_Sym *symtab) {
	int ch;
	int first_statement = 0;

	while((ch = getch()) != 'q') {
		switch(ch) {
			case KEY_UP:
				if (first_statement > 0) {
					// move cursor to previous line
					move(LINES - 1, 0);

					// shift entire window contents up one line
					for (int i = 0; i < COLS * (LINES - 1); i++) 
						addch(inch());
				
					// clearing the last line
					move(LINES - 1, 0);
					clrtoeol();

					// decrement the index of first statement
					first_statement--;
				}
				break;
			case KEY_DOWN:
				if (first_statement < num_entries - LINES) {
					// move the cursor to the next line
					move(LINES - 1, 0);

					// shift the entire window contents down by one line
					for (int i = 0; i < COLS * (LINES - 1); i++)
						addch(inch());

						// clear the last line
					move(LINES - 1, 0);
					clrtoeol();

					// increment the index to the first statement
					first_statement++;
				}
				break;
		}
		// print the visible statements to the screen
		clear();
        wprintw(stdscr, "%4s: %18s%8s%7s%7s%7s %4s %s\n",
        "Num", "Value", "Size", "Type", "Bind", "Vis", "Ndx", "Name");
        wprintw(stdscr, "-------------------------------------------------------\n");   
		for (int i = 0; i < LINES - 1; i++) {
			int statement_index = first_statement + i;
            Elf64_Sym *sym = &symtab[statement_index];
            
            char *symbol_name = &symstrtable[sym->st_name];
			if (statement_index >= 0 && statement_index < num_entries) {
                char *ndx = deal_with_NDX(symtab[statement_index].st_shndx);
                wprintw(stdscr, "%4i: %018lx%8ld %7s%7s %7s %7s %s\n", statement_index, sym->st_value, sym->st_size,
                    STRING_INFO[sym->st_info & 0xf], BIND_TYPES[ELF64_ST_BIND(sym->st_info)], 
                    VISIBILITY[sym->st_other], ndx, symbol_name);
                free(ndx);
            }
		}		
		refresh();
	}
}

/*
*   Deals with the mapping of integer to string to print NDX.
*/
char *deal_with_NDX(uint32_t index) {
    char *snum = malloc(10);
    switch (index) {
        case SHN_UNDEF:    sprintf(snum, "UND"); return snum;
        case SHN_COMMON-1: sprintf(snum, "ABS"); return snum;
        default: sprintf(snum, "%d", index);     return snum;
    }
    return snum;
}


/*
*   If PID is error, this function deals with cleaning up and printing an error message.
*/
void deal_with_error() {
    wprintw(stdscr, "Cannot find PID. Exiting...\n");
    refresh();
    getch();
}


/*
*   print_virtuals: Deals with printing the virtual addresses of some section
*       Pre: None
*       Post: None
*/
void print_virtuals(WINDOW *win, char *pid, V_ADDR v_add[], int num_entries, int lines_per_entry) {
    int ch;
    // int first_statement = 0;
    int total_space = num_entries * lines_per_entry;
    int highlight = 0;
    int i;
    // wbkgdset(win, COLOR_WHITE);

    init_pair(5, COLOR_RED, -1);
    while ((ch = getch()) != 'q') {
        int y = 0, x = 2;
        switch (ch) {
            case KEY_UP:
                if (highlight > 0) {
                    move(LINES - 1, 0);

                    // shift entire window contents up one line
                    for (int i = 0; i < COLS * (LINES - 1); i++) 
                        addch(inch());
                    
                    // clearing the last line
                    move(LINES - 1, 0);
                    clrtoeol();

                    // decrement index of first
                    highlight--;
                }
                break;
            case KEY_DOWN:
                mvwprintw(stdscr, 5, 40, "First_State: %i, y: %i, Lines: %i\n", highlight, getcury(win), LINES - 3);
                if (getcury(win) <= LINES - 3) {
                    if (highlight < total_space - (LINES - 1)) {
                        move(LINES - 1, 0);

                        for (int col = 0; col < COLS * (LINES - 1); col++)
                            addch(inch());

                        move(LINES - 1, 0);
                        clrtoeol();
                    }
                    highlight++;
                } else {
                    break;
                }
        }

        // Clear and redraw the window
        wclear(win);
        i = highlight > LINES / num_entries ? highlight - 1 : 0;
        mvwprintw(stdscr, 7, 40, "i: %i, LINES: %i, highlight: %i\n", i, LINES, highlight);
        for (; i < num_entries; i++) {
            // int statement_index = first_statement + i;
            if (highlight == i + 1) {
                wattron(win, COLOR_PAIR(4));
                mvwprintw(win, y++, x, "%s Virtual Address: \n", v_add[i].name);
                wattroff(win, COLOR_PAIR(4));

                mvwprintw(win, y++, x, "\t[*] Start: %lx\n"    , v_add[i].starting_address);
                mvwprintw(win, y++, x, "\t[*] End:   %lx\n"    , v_add[i].ending_address);
                mvwprintw(win, y++, x, "\t[*] Size:  %li\n"    , v_add[i].ending_address - v_add[i].starting_address);    
                mvwprintw(win, y++, x, "\t[*] Perms: %s\n\n"   , v_add[i].permissions); 
            } else {
                mvwprintw(win, y++, x, "%s Virtual Address: \n", v_add[i].name);
                mvwprintw(win, y++, x, "\t[*] Start: %lx\n"    , v_add[i].starting_address);
                mvwprintw(win, y++, x, "\t[*] End:   %lx\n"    , v_add[i].ending_address);
                mvwprintw(win, y++, x, "\t[*] Size:  %li\n"    , v_add[i].ending_address - v_add[i].starting_address);    
                mvwprintw(win, y++, x, "\t[*] Perms: %s\n\n"   , v_add[i].permissions);                 
            }
/*             if (statement_index >= 0 && statement_index < num_entries) {
                mvwprintw(win, y++, x, "%s Virtual Address: \n", v_add[statement_index].name);
                mvwprintw(win, y++, x, "\t[*] Start: %lx\n"    , v_add[statement_index].starting_address);
                mvwprintw(win, y++, x, "\t[*] End:   %lx\n"    , v_add[statement_index].ending_address);
                mvwprintw(win, y++, x, "\t[*] Size:  %li\n"    , v_add[statement_index].ending_address - v_add[statement_index].starting_address);    
                mvwprintw(win, y++, x, "\t[*] Perms: %s\n\n"   , v_add[statement_index].permissions); 
            } */
        }
        wrefresh(win);
        wmove(win, 0, 0);
    }
}
