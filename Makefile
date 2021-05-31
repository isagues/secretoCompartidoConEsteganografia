#The Target Binary Program
TARGET		:= ss

#The Directories, Source, Includes, Objects, Binary and Resources
export SRCDIR		:= $(CURDIR)/src
export INCDIR		:= $(CURDIR)/inc
export BUILDDIR		:= $(CURDIR)/obj
export TESTDIR		:= $(CURDIR)/test
export TARGETDIR	:= $(CURDIR)
export RESDIR		:= res
export SRCEXT		:= c
export DEPEXT		:= d
export OBJEXT		:= o
MAIN 				:= main

#Flags, Libraries and Includes
STD				?= -std=c99
XFLAGS			?= -ggdb -fsanitize=address -fno-omit-frame-pointer
export CFLAGS	:= -Wextra -Wall -pedantic -Wno-unused-parameter -Wno-newline-eof -Wno-implicit-fallthrough -Wno-unused-function $(XFLAGS) $(STD)
export LIB		:=
export INC		:= -I$(INCDIR)
export INCDEP	:= -I$(INCDIR)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES					:= $(shell find $(SRCDIR) -type f -name *.$(SRCEXT) ! -name *$(MAIN).$(SRCEXT))
export SRC_OBJECTS		:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))
export OBJECTS			:= $(SRC_OBJECTS) $(BUILDDIR)/$(MAIN).$(OBJEXT)
export TARGET			:= $(TARGETDIR)/$(TARGET)

all: directories
	cd $(SRCDIR) && $(MAKE)

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

test:
	cd $(TESTDIR) && $(MAKE)

clean:
	cd $(SRCDIR) && $(MAKE) clean
	cd $(TESTDIR) && $(MAKE) clean

.PHONY: all test clean
