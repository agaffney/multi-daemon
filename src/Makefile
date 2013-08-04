SUBDIRS=httpd common

BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

#default: $(TARGET)
#all: default

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

clean: $(CLEANDIRS)
$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean

