# Definitions
CC = gcc
CC_FLAGS = -Wall -ggdb -UDEBUG
CC_OPTION = -c
OB_OPTION = -o
FILE = testfiles/programs	
BIN_DIR = bin
BINARY = ./HUD

objects = $(addprefix $(BIN_DIR)/, elf.o main.o)


all: elf test



elf:
	$(CC) $(CC_FLAGS) -g elf_runner.c elf_runner.h printing.c printing.h -lncurses $(OB_OPTION) $(addprefix $(BIN_DIR)/, elf.o)

test:
	+$(MAKE) -C $(FILE)

clean:
	rm -f $(objects)