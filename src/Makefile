CC = clang
LD = $(CC)

CFLAGS  = -c -g -Wall -Wextra -std=c99
CFLAGS += $(shell pkg-config glib-2.0 --cflags)

LDFLAGS = $(shell pkg-config glib-2.0 --libs)

OBJECTS = rominfo.o
ROMINFO_OBJECTS = rominfo_main.o

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -o $@

rominfo_main: $(OBJECTS) $(ROMINFO_OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

all: rominfo_main

clean:
	rm -f *.o
	rm -f rominfo
