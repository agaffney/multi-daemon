include Makefile.inc

TARGET=multi-daemon
LIBS=

BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

SRCS    := $(shell find $(SUBDIRS) -name '*.c')
OBJS    := $(patsubst %.c,%.o,$(SRCS))

default: $(TARGET)
all: default

$(SUBDIRS): $(BUILDDIRS)
$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

clean: $(CLEANDIRS)
$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean

$(TARGET): $(BUILDDIRS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

clean:
	-rm -f $(TARGET)
