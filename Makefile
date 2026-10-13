# Compiler
CC = gcc

# Directories
SRC = ./src
INCLUDE = ./include
BIN = ./bin
BUILD = ./build
RESULTS = ./results
INPUT_FILES = ./input_files

# Compiler flags
CFLAGS = -pthread -Wall -g3 -std=c99 -I$(INCLUDE) -fopenmp

# Source files
GAME_SRCS := $(wildcard $(SRC)/game_of_life/*.c)
GAUSS_SRCS := $(wildcard $(SRC)/gauss/*.c)
COMMON_SRC = $(SRC)/common.c

# Object files
GAME_OBJS := $(patsubst $(SRC)/game_of_life/%.c,$(BUILD)/game_of_life_%.o,$(GAME_SRCS))
GAUSS_OBJS := $(patsubst $(SRC)/gauss/%.c,$(BUILD)/gauss_%.o,$(GAUSS_SRCS))
COMMON_OBJ = $(BUILD)/common.o

# Executable names
game_of_life = $(BIN)/game_of_life
gauss = $(BIN)/gauss

# Create directories if they don't exist
.PHONY: directories
directories:
	@mkdir -p $(BIN) $(BUILD) $(RESULTS) $(INPUT_FILES)

# Rule to build object files from source files
$(BUILD)/game_of_life_%.o: $(SRC)/game_of_life/%.c | directories
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/gauss_%.o: $(SRC)/gauss/%.c | directories
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to build the common object file
$(COMMON_OBJ): $(COMMON_SRC) | directories
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to build the game_of_life executable
$(game_of_life): $(GAME_OBJS) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to build the gauss executable
$(gauss): $(GAUSS_OBJS) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Clean rule
.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD) $(RESULTS)

# All rule
.PHONY: all
all: $(game_of_life) $(gauss)