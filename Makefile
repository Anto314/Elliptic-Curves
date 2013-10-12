CC = gcc
CCFLAGS = -W -Wall -g -DDEBUG

SOURCES_DIR = Sources
OBJECTS_DIR = Objects

OBJECTS = $(OBJECTS_DIR)/Elliptic_Curves.o $(OBJECTS_DIR)/Point.o $(OBJECTS_DIR)/Main.o
LIBRARIES = -lgmp
BINARIES = Elliptic_Curves

all: $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o $(BINARIES) $(LIBRARIES)

release: CCFLAGS = -W -Wall -O3 -fexpensive-optimizations -ffast-math -Wl,--strip-all
release: all

$(OBJECTS_DIR)/Elliptic_Curves.o: $(SOURCES_DIR)/Elliptic_Curves.c $(SOURCES_DIR)/Elliptic_Curves.h $(SOURCES_DIR)/Point.h
	$(CC) $(CCFLAGS) -c $(SOURCES_DIR)/Elliptic_Curves.c -o $(OBJECTS_DIR)/Elliptic_Curves.o

$(OBJECTS_DIR)/Point.o: $(SOURCES_DIR)/Point.c $(SOURCES_DIR)/Point.h
	$(CC) $(CCFLAGS) -c $(SOURCES_DIR)/Point.c -o $(OBJECTS_DIR)/Point.o

$(OBJECTS_DIR)/Main.o: $(SOURCES_DIR)/Main.c $(SOURCES_DIR)/Elliptic_Curves.h
	$(CC) $(CCFLAGS) -c $(SOURCES_DIR)/Main.c -o $(OBJECTS_DIR)/Main.o

clean:
	rm -f $(OBJECTS) $(BINARIES)