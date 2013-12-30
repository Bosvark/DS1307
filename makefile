# Makefile - build script */

APPNAME    = DS1307
SOURCEDIR  = ./src
INCLUDEDIR = ./include
OUTPUTDIR  = ./out

# build environment
GNUPREFIX     = arm-unknown-linux-gnueabi

######################################
# common makefile definition section #
######################################

# source files
SOURCES_ASM := $(wildcard *.S)
SOURCES_C   := $(wildcard *.c)
SOURCES_CPP := $(wildcard *.cpp)

# object files
OBJS=\
     $(OUTPUTDIR)/main.o

# Build flags
DEPENDFLAGS := -MD -MP

INCLUDES    += -I$(INCLUDEDIR)

BASEFLAGS   += -O0 -g -fpic -pedantic -pedantic-errors
WARNFLAGS   += -Wall
#WARNFLAGS   += -Werror
ASFLAGS     += $(INCLUDES) $(DEPENDFLAGS) -D__ASSEMBLY__
CFLAGS      += $(INCLUDES) $(DEPENDFLAGS) $(BASEFLAGS) $(WARNFLAGS)

.PHONY: all

all: $(OBJS)
	$(info ********************************* Linking *********************************)
	$(GNUPREFIX)-g++ -o $(OUTPUTDIR)/$(APPNAME) $(OBJS)
	chmod +x $(OUTPUTDIR)/$(APPNAME)

.PHONY: clean

clean:
	rm -f $(OUTPUTDIR)/$(APPNAME)
	rm -f $(OUTPUTDIR)/test 
	rm -f $(OUTPUTDIR)/*.d
	rm -f $(OBJS)

# C.
%.c %.o:
	$(GNUPREFIX)-gcc $(CFLAGS) -c $< -o $@

# C++
%.cpp %.o: | $(OUTPUTDIR)
	$(info ********************************* Compiling C++ *********************************)
	$(GNUPREFIX)-g++ $(CFLAGS) -c $< -o $@

# AS.
%.s %.o:
	$(GNUPREFIX)-gcc $(ASFLAGS) -c $< -o $@

.PHONY:$(OBJDIR)

$(OUTPUTDIR)/main.o   : $(SOURCEDIR)/main.c
