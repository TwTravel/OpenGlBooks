#!/bin/sh
# Copyright 2004, the OpenGLUT contributors.
#
mkdir -p ps
mkdir -p cat3
cd man
ls *.3 | sed 's/.3$//' | awk -v DEV="$1" 'BEGIN{suffix=".ps"; dir="../ps/"; if ("ascii"==DEV){ suffix=".0"; dir="../cat3/"}}{print "groff -mdoc -T " DEV " " $1 ".3 >" dir $1 suffix}' | sh
