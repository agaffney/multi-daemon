CC=gcc
CFLAGS=-g -Wall

SUBDIRS=main echo
INCLUDES=$(patsubst %,-I../%,$(SUBDIRS))

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
