#!/usr/bin/env python

from distutils.core import setup, Extension

setup(name="MathGL",
    version="0.1",
    ext_modules=[Extension("MathGL",
	    ["src/pyinit.cpp"], ["/usr/local/include","/usr/X11R6/include"],
	    library_dirs=["/usr/X11R6/lib","/usr/local/lib"],
	    libraries=["mathglpp", "GL"])])
