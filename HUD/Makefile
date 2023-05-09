#Definitions 
CC = gcc
CC_FLAGS = -Wall -ggdb -UDEBUG -g
CC_OPTION = -c
OB_OPTION = -o
DIR = ..
FILES = $(DIR)/elf_runner.h $(DIR)/elf_runner.c start_up.c contain_files.c logging.c logging.h  \
			contain_files.h display_files.c display_files.h $(DIR)/printing.c $(DIR)/printing.h
NCURSES = -lncurses
PANEL   = -lpanel

all:
	$(CC) $(CC_FLAGS) $(FILES) $(NCURSES) $(PANEL) $(OB_OPTION) run_startup

