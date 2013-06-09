include Makefile.inc

TARGET=multi-daemon
LIBS=

BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

SRCS    := $(shell find $(SUBDIRS) -name '*.c')
OBJREF  := $(subst src,obj,$(SRCS))
OBJS    := $(patsubst %.c,%.o,$(OBJREF))

$(SUBDIRS): $(BUILDDIRS)
$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

clean: $(CLEANDIRS)
$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean

default: $(TARGET)
all: default

$(TARGET): $(BUILDDIRS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

clean:
	-rm -f $(TARGET)
