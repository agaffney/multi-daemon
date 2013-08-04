CC=c99
CFLAGS=-g -Wall
INCLUDE=-I../
LIBS=-pthread

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

COMMONOBJS = $(patsubst %.c, %.o, $(wildcard ../common/*.c))

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

