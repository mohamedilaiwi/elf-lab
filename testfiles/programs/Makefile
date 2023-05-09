# Definitions
CC = gcc
CC_FLAGS = -Wall -ggdb -UDEBUG
CC_OPTION = -c
OB_OPTION = -o
BIN_DIR = ../../bin



test:
	$(CC) $(CC_FLAGS) main.c swap.c $(OB_OPTION) $(addprefix $(BIN_DIR)/, main.o)

