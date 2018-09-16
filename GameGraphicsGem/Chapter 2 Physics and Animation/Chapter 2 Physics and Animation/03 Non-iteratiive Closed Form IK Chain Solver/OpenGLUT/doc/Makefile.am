# Copyright 2004, the OpenGLUT contributors.
#
EXTRA_DIST = \
	doxygen.cfg dox.awk make-man.sh make-roff.sh

all:

documentation: man dox


clean:
	rm -Rf html latex man cat3 ps

# Update the OpenGLUT website from the locally generated
# HTML documentation

# We observed permissions issues with scp - rsync is faster anyway
# scp -p html/* shell.sourceforge.net:/home/groups/o/op/openglut/htdocs

web:
	rsync -avz --rsh=ssh html/* shell.sourceforge.net:/home/groups/o/op/openglut/htdocs/

man: man-ascii man-ps

man-ascii: roff
	sh make-man.sh ascii

man-ps: roff
	sh make-man.sh ps

dox:
	rm -Rf html latex
	doxygen doxygen.cfg
#	cp images/*.png html/
#	cp images/*.jpg html/
	chmod g+w html/*

roff:
	sh make-roff.sh

install: man-ascii
	$(INSTALL) -d $(mandir)/man3 man
	$(INSTALL) -d $(mandir)/cat3 cat3
	$(INSTALL) man/* $(mandir)/man3
	$(INSTALL) cat3/* $(mandir)/cat3
