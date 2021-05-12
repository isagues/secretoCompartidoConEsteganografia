# Subfolders
SRC  			= src
TEST 			= test

export PROGRAM  = ss

export MAIN 	= main

# Todos los archivos *.c que no sean MAIN
SOURCES 		:= $(shell find $(CURDIR)/$(SRC) -name '*.c' ! -name *$(MAIN).c -type f)

XFLAGS 			?= -ggdb -fsanitize=address -fno-omit-frame-pointer
export CFLAGS	:= -Wextra -Wall -pedantic -Wno-unused-parameter -Wno-unused-function -Wno-implicit-fallthrough -I$(CURDIR)/$(SRC) $(XFLAGS)

export TARGET 	:= $(CURDIR)/$(PROGRAM)

# Discriminamos MAIN para evitar que colisione con tests
export MAIN_OBJECT = $(MAIN).o
export SRC_OBJECTS := $(SOURCES:.c=.o)

all:
	cd $(SRC) && $(MAKE)

$(TEST):
	cd $(TEST) && $(MAKE)

clean:
	cd $(SRC) && $(MAKE) clean
	cd $(TEST) && $(MAKE) clean

.PHONY: all $(TEST) clean
