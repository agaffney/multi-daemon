TARGET=httpd

include ../Makefile.inc

#default: $(TARGET)
default: $(OBJECTS)
all: default

clean:
	-rm -f $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $@
