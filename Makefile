CC = gcc
CCFLAGS = -W -Wall -g -DDEBUG

SOURCES_DIR = Sources
OBJECTS_DIR = Objects

OBJECTS_SHARED = $(OBJECTS_DIR)/Elliptic_Curves.o $(OBJECTS_DIR)/Point.o $(OBJECTS_DIR)/Network.o
#$(OBJECTS_DIR)/Main.o
OBJECTS_DIFFIE_HELLMAN = $(OBJECTS_DIR)/Diffie_Hellman.o
LIBRARIES = -lgmp

all: $(OBJECTS_SHARED) $(OBJECTS_DIFFIE_HELLMAN)
	@# Compile classic Diffie-Hellman algorithm
	$(CC) $(CCFLAGS) $(OBJECTS_SHARED) $(OBJECTS_DIFFIE_HELLMAN) -o Diffie_Hellman $(LIBRARIES)

release: CCFLAGS = -W -Wall -O3 -fexpensive-optimizations -ffast-math -Wl,--strip-all
release: all

#---------------------------------------------------------------------------------------------------------------------------------------------------
# Base objects used by all programs
#---------------------------------------------------------------------------------------------------------------------------------------------------
$(OBJECTS_DIR)/Elliptic_Curves.o: $(SOURCES_DIR)/Elliptic_Curves.c $(SOURCES_DIR)/Elliptic_Curves.h $(SOURCES_DIR)/Point.h
	$(CC) $(CCFLAGS) -c $(SOURCES_DIR)/Elliptic_Curves.c -o $(OBJECTS_DIR)/Elliptic_Curves.o

$(OBJECTS_DIR)/Point.o: $(SOURCES_DIR)/Point.c $(SOURCES_DIR)/Point.h
	$(CC) $(CCFLAGS) -c $(SOURCES_DIR)/Point.c -o $(OBJECTS_DIR)/Point.o

$(OBJECTS_DIR)/Network.o: $(SOURCES_DIR)/Network.c $(SOURCES_DIR)/Network.h
	$(CC) $(CCFLAGS) -c $(SOURCES_DIR)/Network.c -o $(OBJECTS_DIR)/Network.o



#$(OBJECTS_DIR)/Main.o: $(SOURCES_DIR)/Main.c $(SOURCES_DIR)/Elliptic_Curves.h
#	$(CC) $(CCFLAGS) -c $(SOURCES_DIR)/Main.c -o $(OBJECTS_DIR)/Main.o

#---------------------------------------------------------------------------------------------------------------------------------------------------
# Diffie-Hellman key exchanging
#---------------------------------------------------------------------------------------------------------------------------------------------------
$(OBJECTS_DIR)/Diffie_Hellman.o: $(SOURCES_DIR)/Diffie_Hellman.c $(SOURCES_DIR)/Elliptic_Curves.h
	$(CC) $(CCFLAGS) -c $(SOURCES_DIR)/Diffie_Hellman.c -o $(OBJECTS_DIR)/Diffie_Hellman.o

clean:
	rm -f $(OBJECTS_SHARED) $(OBJECTS_DIFFIE_HELLMAN) $(BINARIES)