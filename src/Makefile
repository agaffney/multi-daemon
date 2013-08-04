SUBDIRS=http echo common

BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

SRCS    := $(shell find $(SUBDIRS) -name '*.c')
OBJS    := $(patsubst %.c,%.o,$(SRCS))

#default: $(TARGET)
#all: default

$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

clean: $(CLEANDIRS)
$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean

$(SUBDIRS): $(@:%=build-%)

