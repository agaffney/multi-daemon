TARGET=httpd

include ../Makefile.inc

default: $(TARGET)
all: default

clean:
	-rm -f $(OBJECTS) $(TARGET)

$(COMMONOBJS):
	$(MAKE) -C ../common

$(TARGET): $(OBJECTS) $(COMMONOBJS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) $(COMMONOBJS) -o $@
