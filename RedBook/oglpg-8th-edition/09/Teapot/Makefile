#! make

TARGETS = $(basename $(wildcard *.cpp))

LOAD_SHADERS.src = ../../lib/LoadShaders.cpp
LOAD_SHADERS.obj = LoadShaders.o

CXXOPTS = -mno-cygwin -mwin32
CXXDEFS = 
CXXINCS = -I../../include

ifdef DEBUG
CXXDEFS += -DDEBUG
endif

CXXFLAGS = $(CXXOPTS) $(CXXDEFS) $(CXXINCS)

LDOPTS =
LDDIRS = -L../../lib
LDLIBS = -lfreeglut -lglew32 -lopengl32 -lgdi32 -lwinmm

LDFLAGS = $(LDOPTS) $(LDDIRS) $(LDLIBS)

RM   = rm -fr
DIRT = $(wildcard *~ *.o)
DIRT += ipch Debug $(wildcard Teapot.sdf Teapot.suo Teapot.vcxproj.*)

#-----------------------------------------------------------------------------

default all: $(TARGETS)

%: %.cpp $(LOAD_SHADERS.obj)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

%.i: %.cpp
	$(CXX) $(CXXFLAGS) -E $< | uniq > $@

$(LOAD_SHADERS.obj): $(LOAD_SHADERS.src)
	$(CXX) $(CXXFLAGS) -c $(LOAD_SHADERS.src)

#-----------------------------------------------------------------------------

clean:
	$(RM) $(DIRT)

clobber: clean
	$(RM) $(TARGETS)

