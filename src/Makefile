include Makefile.inc

TARGETS=httpd
SUBDIRS=$(TARGETS) common

BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

COMMONOBJS = $(patsubst %.c, %.o, $(wildcard common/*.c))

default:
all:

# Cancel default rule for compiling
%.o: %.c

.SECONDEXPANSION:
.PHONY: $(SUBDIRS)

$(TARGETS): clean-common clean-$$@ build-common $$(patsubst %,build-%,$$(@)) $$@/$$@

# foo/foo targets
$(TARGETS:%=%/%):
	$(CC) $(CFLAGS) $(LIBS) $(patsubst %.c, %.o, $(wildcard $(dir $@)/*.c)) $(COMMONOBJS) -o $(@)

$(patsubst %.c, %.o, $(shell find -name '*.c')):
	$(MAKE) -C $(dir $@)

$(BUILDDIRS): $$(patsubst %.c, %.o, $$(wildcard $$(patsubst build-%, %, $$@)/*.c))

clean: $(CLEANDIRS)
$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean

