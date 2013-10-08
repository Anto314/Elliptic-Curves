CC = gcc
CCFLAGS = -W -Wall -O3 -fexpensive-optimizations -ffast-math -Wl,--strip-all

SOURCES_DIR = Sources
OBJECTS_DIR = Objects

OBJECTS = $(OBJECTS_DIR)/Elliptic_Curves.o
LIBRARIES = -lgmp
BINARIES = Elliptic_Curves

all: $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o $(BINARIES) $(LIBRARIES)

$(OBJECTS_DIR)/Elliptic_Curves.o: $(SOURCES_DIR)/Elliptic_Curves.c $(SOURCES_DIR)/Elliptic_Curves.h
	$(CC) $(CCFLAGS) -c $(SOURCES_DIR)/Elliptic_Curves.c -o $(OBJECTS_DIR)/Elliptic_Curves.o