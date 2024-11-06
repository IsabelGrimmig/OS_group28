CC = gcc
CCWARNINGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function
CCOPTS = -g -O0
CFLAGS = $(CCWARNINGS) $(CCOPTS)

LIB_SOURCES = aq_tsafe.c
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)
LIB = aq
LIB_DIR = mylib
LIB_NAME = lib$(LIB).a

DEMO_SOURCES = aq_demo.c aux_new.c
DEMO_OBJECTS = $(DEMO_SOURCES:.c=.o)

TEST_FILE = aq_test.c
TEST_SOURCES = $(TEST_FILE) aux_new.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

DEMO_EXECUTABLE = demo
TEST_EXECUTABLE = test

EXECUTABLES = $(DEMO_EXECUTABLE) $(TEST_EXECUTABLE)

.PHONY: all lib clean clean-all

all: lib $(DEMO_EXECUTABLE) $(TEST_EXECUTABLE)

lib: $(LIB_DIR)/$(LIB_NAME)

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR)/$(LIB_NAME): $(LIB_OBJECTS)
	mkdir -p $(LIB_DIR)
	ar -rcs $@ $^

$(DEMO_EXECUTABLE): $(DEMO_OBJECTS) $(LIB_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) $(DEMO_OBJECTS) -L$(LIB_DIR) -l$(LIB) -o $@ 

$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(LIB_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) -lpthread -L$(LIB_DIR) -l$(LIB) -o $@ 

clean:
	rm -rf *.o *~ 

clean-all: clean
	rm -rf $(LIB_DIR) $(EXECUTABLES)
