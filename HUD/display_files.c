#include "display_files.h"
#include "logging.h"
#include "contain_files.h"
#include "../elf_runner.h"

int NUM_WINDOWS = 2;
short COLOR_BEIGE  = 0;
short COLOR_RANDOM = 0;


char *FILE_OPTIONS[] = {
					"Return",
                    "Display Information",
                    "See Section Header",
                    "See Metadata",
                    "Find Relocation Table",
                    "Show Dynamic Linker Info",
                    "See Visual Representation",
                };


static char *ELF_RUNNER_SOURCE = "../elf_runner.c ";
static char *ELF_RUNNER_HEADER = "../elf_runner.h ";
static char *ELF_PRINT_SOURCE  = "../printing.c ";
static char *ELF_PRINT_HEADER  = "../printing.h "; 

/*
*	Initializes ncurses terminal: raw buffering
*/
void init_terminal() {
	initscr();
	clear();
	noecho();
	cbreak();
	start_color();
}


/*
*	Creates all unique color pairs for terminal:
*	Pre: None
*	Post: None
*/
void create_color_pairs() {
	init_pair(2, COLOR_YELLOW, COLOR_CYAN);
}

/*
 * Driver for displaying the HUD
 */
void display_hud(DIRECTORY *directory) {
	WINDOWS *windows[2];
	int c, row, col, choice = 0, highlight = 1;
	int NUM_FILES = directory->NUM_FILES;
	struct dirent **files = directory -> files; 

	if (!has_colors()) {
		endwin();
		printf("Terminal does not support color\n");
		return;
	}


    // Enable color support and define color pairs
	// start_color();
	// create_color_pairs();
	// use_default_colors();

	getmaxyx(stdscr, row, col);
	init_windows(windows, NUM_WINDOWS, row, col);

	init_color(COLOR_BEIGE, 1000, 922, 726);
	bkgd(COLOR_PAIR(COLOR_BEIGE));

	mvprintw(0, 0, "Use arrow keys to move down section. Press F1 to Exit.\n");
	refresh();
	print_files(windows[0]->win, files, highlight, NUM_FILES);
	// print_files(windows[1]->win, files, highlight);
	while(1) {
		c = wgetch(windows[0]->win);
		deal_with_highlight(c, &highlight, &choice, NUM_FILES);
		print_files(windows[0]->win, files, highlight, NUM_FILES);
		print_hover(windows, directory, highlight);
		if (choice != 0)
			break;
	}
	select_choice(windows, directory, choice);

	refresh();
	endwin();
}

/*
*	print_choice: Deals with printing whatever user hovers over.
*		IF  executable: Provides options to show with file
*		else directory: Show preview to files inside.
*	Pre: None
*	Post: None
*/
void print_hover(WINDOWS *win[], DIRECTORY *directory, int highlight) {
	char *hovered_file_name = directory->files[highlight - 1] -> d_name;
	werase(win[1]->win);
	box(win[1]->win, 0, 0);
	if (is_dir(hovered_file_name)) { // This is a directory, showcase next.
		// char *select_choice = malloc()
		DIRECTORY *temp_dir = malloc(sizeof(DIRECTORY));
		get_files(temp_dir, hovered_file_name);
		print_files(win[1]->win, temp_dir->files, highlight, temp_dir->NUM_FILES);
		// free_array(temp_dir->files, temp_dir->NUM_FILES);		// frees the files member
		free(temp_dir);					// frees the actual directory
	} else {
		print_options(win, 1, highlight, false);
	}
}



/*
*	init_windows: Allocates memory for WINDOWS *win
*	Pre: None
*	Post: Returns pointer to an allocated WINDOWS array object
*/
void init_windows(WINDOWS *windows[], int NUM_WINDOWS, int row, int col) {
	for (int i = 0; i < NUM_WINDOWS; i++) 
		windows[i] = malloc(sizeof(WINDOWS));

	windows[0]->height = (row - 2);
	windows[0]->width  = (col * 0.23);
	windows[0]->startx = 1;
	windows[0]->starty = 1;
	
	windows[1]->height = windows[0]->height;
	windows[1]->width  = windows[0]->width;
	windows[1]->startx = (windows[0]->startx) + (windows[0]->width) + 1;
	windows[1]->starty = (windows[0]->starty);

	// attron(COLOR_PAIR(2));
	windows[0]->win = newwin(windows[0]->height, windows[0]->width, windows[0]->starty, windows[0]->startx);
	windows[1]->win = newwin(windows[1]->height, windows[1]->width, windows[1]->starty, windows[1]->startx);
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	wbkgd(windows[0]->win, COLOR_PAIR(1));

	keypad(windows[0]->win, TRUE);
	keypad(stdscr, TRUE);
}


/*
*	Select_Choice: Deals with the choice user selected. 
*	Pre: None
*   Post: Changes VIEW depending on selected choice
*/
void select_choice(WINDOWS *windows[], DIRECTORY *directory, int choice) {
	// printf("User selected: %s\n", (directory->files)[choice-1]->d_name);
	struct dirent **file_names = directory->files;
	if (is_dir(file_names[choice - 1]->d_name)) {
		char *selected_choice = malloc(strlen(file_names[choice-1]->d_name) + 1);
		memcpy(selected_choice, file_names[choice - 1]->d_name, strlen(file_names[choice-1]->d_name));
		// free_array(file_names, directory->NUM_FILES);
		get_files(directory, selected_choice);
		display_hud(directory);
	} else {
		clear();
		char *selected_file = file_names[choice - 1] -> d_name;
		int c, choice = 0, highlight = 1, NUM_OPTIONS = 7;
		WINDOW *win = windows[0]->win;
		wclear(win);
		print_options(windows, 0, highlight, true);
		box(win, 0, 0);
		refresh();

		while(1) {
			c = wgetch(win);
			deal_with_highlight(c, &highlight, &choice, NUM_OPTIONS);
			print_options(windows, 0, highlight, true);
			if (choice != 0)
				break;
		}
		if (strcmp(file_names[choice]->d_name, "..") == 0) {
			get_files(directory, ".");
			display_hud(directory);
		} else {
			display_file_option(windows, directory, selected_file, choice);
		}
	}
}

/*
*	print_options: Prints a series of options that a user can use 
*		on executable file. 
*	Shown on the right HUD when hovering over a file.
*	Pre: None
*	Post: None
*	Return visual HUD
*/
void print_options(WINDOWS *windows[], int win_number, int highlight, bool main_hud) {
	int x, y, i;
	int NUM_OPTIONS = 7;
	WINDOW *win = windows[win_number]->win;
	
	// find coordinates to print inside window
	x = 2;
	y = 2;
	for (i = 0; i < NUM_OPTIONS; i++) {
        if (highlight == i + 1) {
			bool is_directory = is_dir(FILE_OPTIONS[i]);
            wattron(win,  change_color(is_directory));
			mvwprintw(win, y, x, "%s",  FILE_OPTIONS[i]);
			wattroff(win, change_color(is_directory));
        } else {
            mvwprintw(win, y, x, "%s", FILE_OPTIONS[i]);
        }
        ++y;
	}
	wrefresh(win);
}



/*
 * Helper function to print all files inside the box.
 */
void print_files(WINDOW *win, struct dirent **files, int highlight, int NUM_FILES) {
    int x, y, i;

    x = 2;
    y = 1;
    box(win, 0, 0);
	struct dirent *file;
    for (i = 0; i < NUM_FILES; i++) {
        file = files[i];
        if (highlight == i + 1) {
            wattron(win, A_REVERSE);
			mvwprintw(win, y, x, "%s",  file->d_name);
			wattroff(win, A_REVERSE);
		} else {
            mvwprintw(win, y, x, "%s",  file->d_name);
        }
        ++y;
    }
    wrefresh(win);
}


int change_color(bool is_directory) {
	init_pair(1, COLOR_RED, -1);
	return is_directory ? COLOR_PAIR(1) | A_REVERSE : A_REVERSE;
}


void deal_with_highlight(int c, int *highlight, int *choice, int NUM_FILES) {
	switch(c) {
		case KEY_UP:
			if (*highlight == 1)
				*highlight = NUM_FILES;
			else
				--(*highlight);
			break;
		case KEY_DOWN:
			if (*highlight == NUM_FILES)
				*highlight = 1;
			else
				++(*highlight);
			break;
		case 10:
			*choice = *highlight;
			break;
	}
}


/*
*	Helper function that determines which choice user selected.
*		
*/
void display_file_option(WINDOWS *windows[], DIRECTORY *directory, char *selected_file, int choice) {
	char compile_cmd[256], path_of_dir[256], files[directory->NUM_FILES * 20]; 
	char *absolute_path = realpath(selected_file, NULL);
	strncpy(path_of_dir, absolute_path, strlen(absolute_path) - strlen(selected_file));
	strcat(path_of_dir, "temp.o");
	strcpy(files, "");
	for (int i = 0; i < directory->NUM_FILES; i++) {
		if (!is_dir((directory->files)[i]->d_name) && strcmp((directory->files)[i]->d_name, "run_startup")
			&& strcmp((directory->files)[i]->d_name, "Makefile")) {
			strcat(files, (directory->files)[i]->d_name);
			strcat(files, " ");	
		}
	}
	strcat(files, ELF_RUNNER_SOURCE);
	strcat(files, ELF_RUNNER_HEADER);
	strcat(files, ELF_PRINT_SOURCE);
	strcat(files, ELF_PRINT_HEADER);
	strcat(files, "-lncurses");
	
	snprintf(compile_cmd, 256, "gcc %s -o temp.o", files);
	system(compile_cmd);
	create_elf(path_of_dir);
	switch(choice) {
		case 2:	display_information		 (windows, selected_file); break;
		case 3: see_section_header		 (windows, selected_file); break;
		case 4: see_metadata             (windows, selected_file); break;
		case 5: find_relocation_table    (windows, selected_file); break;
		case 6: show_dynamic_linker_info (windows, selected_file); break;
		case 7: see_visual_representation(windows, selected_file); break;
		default: 										  		   break;
	}
	system("rm -f temp.o");
	free(absolute_path);
}





/*
*	display_information: Displays information about an executable files: size, arch, machine, etc..
*		Pre: File exists in directory
*		Post: Prints information about executable file size, arch, machine, etc..
*/
void display_information(WINDOWS *windows[], char *file) {
	clear();
	use_default_colors();
	init_pair(1, COLOR_RED, -1);
    wattron(stdscr, COLOR_PAIR(1));
	get_symbol_name(strtab);
	wattroff(stdscr, COLOR_PAIR(1));
}

void see_section_header(WINDOWS *windows[], char *file) {

}

void see_metadata(WINDOWS *windows[], char *file) {

}

void find_relocation_table(WINDOWS *windows[], char *file) {

}

void show_dynamic_linker_info(WINDOWS *windows[], char *file) {

}

void see_visual_representation(WINDOWS *windows[], char *file) {

}