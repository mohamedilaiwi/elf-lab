#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <dirent.h>
#include <unistd.h>




typedef struct _WINDOWS {
    int startx, starty, width, height;
    char label[20];
    int label_color;
    WINDOW *win;
} WINDOWS;


extern int NUM_WINDOWS;    // win[0] is HUD to select file, win[1] is index to choose what to do with file
struct _DIRECTORY;

void init_terminal              ();
void print_files                (WINDOW *win, struct dirent **files, int highlight, int NUM_FILES);
void print_hover                (WINDOWS *win[], struct _DIRECTORY *directory, int highlight);   
void select_choice              (WINDOWS *win[], struct _DIRECTORY *directory, int);
void create_color_pairs         ();
void init_windows               (WINDOWS *win[], int NUM_WINDOWS, int row, int col);
void print_options              (WINDOWS *win[], int win_number, int highlight, bool main_hud);
void deal_with_highlight        (int c, int *highlight, int *choice, int NUM_FILES); 
void display_file_option        (WINDOWS *windows[], struct _DIRECTORY *directory, char *selected_file, int choice);

void display_information        (WINDOWS *windows[], char *);
void see_section_header         (WINDOWS *windows[], char *);
void see_metadata               (WINDOWS *windows[], char *);
void find_relocation_table      (WINDOWS *windows[], char *);
void show_dynamic_linker_info   (WINDOWS *windows[], char *);
void see_visual_representation  (WINDOWS *windows[], char *);