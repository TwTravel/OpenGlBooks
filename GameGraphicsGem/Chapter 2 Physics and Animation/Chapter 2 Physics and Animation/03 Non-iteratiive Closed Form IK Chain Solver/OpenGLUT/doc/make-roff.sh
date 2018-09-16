#!/bin/sh
# Copyright 2004, the OpenGLUT contributors.
#
rm -rf man
mkdir man

for f in ../src/*.c
do
	awk -f dox.awk <$f
done
for f in man/*.3
do
	sed <$f '/^$/d' >$f.new
	mv $f.new $f
done

