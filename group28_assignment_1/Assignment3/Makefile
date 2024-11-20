# Compiler and options
CC = gcc
CCWARNINGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function
CCOPTS = -g -O0
CFLAGS = $(CCWARNINGS) $(CCOPTS)

# Library sources and objects
LIB_SOURCES = aq_tsafe.c
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)
LIB = aq
LIB_DIR = mylib
LIB_NAME = lib$(LIB).a

# Demo sources and objects
DEMO_SOURCES = aq_demo.c aux_new.c
DEMO_OBJECTS = $(DEMO_SOURCES:.c=.o)

# Test sources and objects
TEST_SOURCES = aq_test.c aux_new.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

# Executables to build
DEMO_EXECUTABLE = demo.exe
TEST_EXECUTABLE = test.exe

# Phony targets for make commands
.PHONY: all lib clean clean-all

# Default target to build everything
all: lib $(DEMO_EXECUTABLE) $(TEST_EXECUTABLE)

# Build the static library
lib: $(LIB_DIR)/$(LIB_NAME)

# Compile object files from C source files
%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into a static library
$(LIB_DIR)/$(LIB_NAME): $(LIB_OBJECTS)
	mkdir -p $(LIB_DIR)
	ar -rcs $@ $^

# Link demo objects with the static library to create the demo executable
$(DEMO_EXECUTABLE): $(DEMO_OBJECTS) $(LIB_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) $(DEMO_OBJECTS) -L$(LIB_DIR) -l$(LIB) -o $@ 

# Link test objects with the static library and pthread to create the test executable
$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(LIB_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) -lpthread -L$(LIB_DIR) -l$(LIB) -o $@ 

# Clean up object files and temporary files
clean:
	rm -rf *.o *~ 

# Additional clean-up for all generated files
clean-all: clean
	rm -rf $(LIB_DIR) $(EXECUTABLES)
