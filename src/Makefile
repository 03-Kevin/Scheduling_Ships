# Compiler and flags
CC = gcc
CFLAGS = -Wall -I./ce_threads -I./ce_mutex -I./calendar

# Source files
SRC = ./program/menu.c ./ce_threads/ce_thread.c ./ce_mutex/ce_mutex.c ./program/boat_manager.c ./calendar/calendarizador.c

# Object files (same as source files but with .o extension)
OBJ = $(SRC:.c=.o)

# Output executable
TARGET = ./program/menu

# Default target to build the project
all: $(TARGET)

# Rule to link the object files and create the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule to compile each .c file into a .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJ) $(TARGET)
