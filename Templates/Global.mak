#*******************************************************************************
# Copyright (C) Martin Laporte.
#
# General makefile template.
#
# Assumes the following input:
#
# SOURCES - List of all sources.
# OUTPUTS - Output files.
#*******************************************************************************

#*******************************************************************************
# Configuration settings.
#*******************************************************************************

# Define the name of the tools used for compilation
export CC			:= gcc
export CXX			:= g++
export LD			:= ld
export OBJCOPY			:= objcopy
export AR			:= ar

# Define the flags used for compilation
export CPPFLAGS     := -D_DEBUG \
                       -D_INTEL386_ \
                       -D_STLP_NO_IOSTREAMS \
                       -D_STLP_USE_NEWALLOC \
		       -fpermissive \
                       -I$(NUTSHELL)/Kernel \
                       -isystem $(NUTSHELL)/External/STLPort \
                       -isystem $(NUTSHELL)/External/Boost
export CFLAGS		:= -Werror
export CXXFLAGS		:= $(CFLAGS) -Wreorder -Wno-deprecated
export LDFLAGS		:= -L /usr/lib/gcc/i686-pc-cygwin/4.5.3/

#*******************************************************************************
# Global rules
#*******************************************************************************

# Create the list of object files.
OBJECTS := $(SOURCES:.S=.o)
OBJECTS := $(OBJECTS:.c=.o)
OBJECTS := $(OBJECTS:.cpp=.o)

# Compute outputs for libraries
ifdef LIBRARY
OUTPUTS := $(LIBRARY)
endif

# Compute outputs for kernel
ifdef KERNEL
MAP     := $(KERNEL:.bin=.map)
EXEC    := $(KERNEL:.bin=.exe)
OUTPUTS := $(KERNEL) $(MAP)
LIBRARIES = $(foreach dir,$(SUBDIRS),$(dir)/$(dir).a)
endif

# Have the global rules propagate to subdirs
all deps clean distclean : $(SUBDIRS)

# This rule builds everything
.PHONY : all
all : $(OUTPUTS)

# This rule rebuilds dependencies
.PHONY : deps
deps :
	$(CC) $(CPPFLAGS) -MM $(SOURCES) > Dependencies.dep

# This rule cleans the directory
.PHONY : clean
clean :
	rm -f $(OBJECTS)
	rm -f $(OUTPUTS)

# This rule really cleans the directory
.PHONY : distclean
distclean : clean
	rm -f Dependencies.dep

# This rule recurses into subdirectories
.PHONY : $(SUBDIRS)
$(SUBDIRS) :
	$(MAKE) $(MAKECMDGOALS) -C $@

# Build libraries
ifdef LIBRARY
$(LIBRARY) : $(OBJECTS)
	rm -f $(LIBRARY)
	$(AR) -r $(LIBRARY) $(OBJECTS)
endif

# Build kernel
ifdef KERNEL
$(KERNEL) $(EXEC) $(MAP) : $(OBJECTS) $(LIBRARIES)
	$(LD) $(LDFLAGS) $(OBJECTS) --whole-archive $(LIBRARIES) --no-whole-archive -lgcc -lstdc++ -o $(EXEC) -T Kernel.ld -Map $(MAP)
	$(OBJCOPY) $(EXEC) $(KERNEL) -j .text -j .data -j .bss -O binary --set-start 0xC0000000
endif

-include Dependencies.dep
