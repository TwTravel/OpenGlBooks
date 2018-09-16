## Portions copyright (C) 2004, the OpenGLUT project contributors.
## OpenGLUT branched from freeglut in February, 2004.
## Process this file with automake to produce Makefile.im

#
# The library we want to build
#
lib_LTLIBRARIES = libopenglut.la

noinst_HEADERS = og_internal.h
#
# Those source files build the OpenGLUT library
#
libopenglut_la_SOURCES = og_callbacks.c \
			 og_cursor.c \
			 og_display.c \
			 og_ext.c \
			 og_font.c \
			 og_font_data.c \
			 og_stroke_roman.c \
			 og_stroke_mono_roman.c \
			 og_gamemode.c \
			 og_geometry.c \
			 og_init.c \
			 og_joystick.c \
			 og_main.c \
			 og_menu.c \
			 og_misc.c \
			 og_overlay.c \
			 og_state.c \
			 og_structure.c \
			 og_teapot.c \
			 og_videoresize.c \
			 og_window.c

# The above list has had this file excised as obsolete:
#			 og_glutfont_definitions.c \
#

#
# Additional linker flags
#	 
libopenglut_la_LIBADD = $(LIBM) $(X_LIBS) -lGL -lGLU -lXext -lX11 $(LIBXXF86VM)
libopenglut_la_LDFLAGS = -version-info 1:0:0
libopenglut_la_CFLAGS = $(X_CFLAGS)

INCLUDES = -I../include

