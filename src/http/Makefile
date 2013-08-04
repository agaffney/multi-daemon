TARGET=httpd

include ../Makefile.inc

default: $(TARGET)
#default: $(OBJECTS)
all: default

clean:
	-rm -f $(OBJECTS) $(TARGET)

$(COMMONOBJS):
	$(MAKE) -C ../common

$(TARGET): $(OBJECTS) $(COMMONOBJS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) $(COMMONOBJS) -o $@
