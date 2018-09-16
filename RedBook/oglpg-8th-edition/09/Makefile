#! make

SUBDIRS = $(filter-out $(wildcard Makefile*), $(wildcard *))

DIRT = $(wildcard *~)

#-----------------------------------------------------------------------------
.PHONY: default all clean clobber

default all clean clobber::
	for d in $(SUBDIRS) ; do ( cd $$d ; $(MAKE) $@ ) ; done

clean clobber::
	$(RM) $(DIRT)
