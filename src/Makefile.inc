CC=gcc
CFLAGS=-g -Wall

SUBDIRS=common echo http
INCLUDES=$(patsubst %,-I../%,$(SUBDIRS))

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

