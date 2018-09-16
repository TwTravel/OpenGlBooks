#! make

SUBDIRS = 01 10

DIRT = $(wildcard *~)

#-----------------------------------------------------------------------------
.PHONY: default all clean clobber

default all clean clobber::
	for d in $(SUBDIRS) ; do ( cd $$d ; $(MAKE) $@ ) ; done

clean clobber::
	$(RM) $(DIRT)
