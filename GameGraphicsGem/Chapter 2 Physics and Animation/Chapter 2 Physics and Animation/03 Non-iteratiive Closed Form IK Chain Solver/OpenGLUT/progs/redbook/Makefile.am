EXTRA_DIST = \
	aaindex.c bezcurve.c double.c light.c pickdepth.c sccolorlight.c surface.c \
	texsub.c aapoly.c bezmesh.c drawf.c lines.c picksquare.c scenebamb.c teaambient.c texturesurf.c \
	aargb.c checker.c feedback.c list.c plane.c scene.c teapots.c torus.c \
	accanti.c clip.c fog.c material.c planet.c sceneflat.c tess.c trim.c \
	accpersp.c colormat.c fogindex.c mipmap.c polyoff.c select.c tesswind.c unproject.c \
	alpha3D.c cube.c font.c model.c polys.c smooth.c texbind.c varray.c \
	alpha.c depthcue.c hello.c movelight.c quadric.c stencil.c texgen.c wrap.c \
	anti.c dof.c image.c nurbs.c robot.c stroke.c texprox.c \
	jitter.h

noinst_PROGRAMS = \
	aaindex bezcurve double light pickdepth sccolorlight surface \
	texsub aapoly bezmesh drawf lines picksquare scenebamb teaambient texturesurf \
	aargb checker feedback list plane scene teapots torus \
	accanti clip fog material planet sceneflat tess trim \
	accpersp colormat fogindex mipmap polyoff select tesswind unproject \
	alpha3D cube font model polys smooth texbind varray \
	alpha depthcue hello movelight quadric stencil texgen wrap \
	anti dof image nurbs robot stroke texprox 

# Must NOT override LDFLAGS, since it is a user variable,
# and some systems need to modify it.  Presently, the ugly
# hack to work around this is to specify a per-file LDFLAGS.
# For some reason, AM_LDFLAGS does not work on LINUX.
#
# MUST override CFLAGS, since the user may have specified
# -Wall -Werror, which will cause problems with one of the
# demos.  (The -Wall -Werror condition is the error checking
# that OpenGLUT enables.  See --disable-warnings and
# --enable-warnings.)
#
#LDFLAGS = ../../src/lib@LIBRARY@.la
CFLAGS  = $(X_CFLAGS)
INCLUDES    = -I$(top_srcdir)/include

aaindex_SOURCES      = aaindex.c
aaindex_LDFLAGS      = ../../src/lib@LIBRARY@.la
bezcurve_SOURCES     = bezcurve.c
bezcurve_LDFLAGS     = ../../src/lib@LIBRARY@.la
double_SOURCES       = double.c
double_LDFLAGS	     = ../../src/lib@LIBRARY@.la
light_SOURCES        = light.c
light_LDFLAGS	     = ../../src/lib@LIBRARY@.la
pickdepth_SOURCES    = pickdepth.c
pickdepth_LDFLAGS    = ../../src/lib@LIBRARY@.la
sccolorlight_SOURCES = sccolorlight.c
sccolorlight_LDFLAGS = ../../src/lib@LIBRARY@.la
surface_SOURCES      = surface.c
surface_LDFLAGS	     = ../../src/lib@LIBRARY@.la
texsub_SOURCES       = texsub.c
texsub_LDFLAGS	     = ../../src/lib@LIBRARY@.la
aapoly_SOURCES       = aapoly.c
aapoly_LDFLAGS	     = ../../src/lib@LIBRARY@.la
bezmesh_SOURCES      = bezmesh.c
bezmesh_LDFLAGS	     = ../../src/lib@LIBRARY@.la
drawf_SOURCES        = drawf.c
drawf_LDFLAGS	     = ../../src/lib@LIBRARY@.la
lines_SOURCES        = lines.c
lines_LDFLAGS	     = ../../src/lib@LIBRARY@.la
picksquare_SOURCES   = picksquare.c
picksquare_LDFLAGS   = ../../src/lib@LIBRARY@.la
scenebamb_SOURCES    = scenebamb.c
scenebamb_LDFLAGS    = ../../src/lib@LIBRARY@.la
teaambient_SOURCES   = teaambient.c
teaambient_LDFLAGS   = ../../src/lib@LIBRARY@.la
texturesurf_SOURCES  = texturesurf.c
texturesurf_LDFLAGS  = ../../src/lib@LIBRARY@.la
aargb_SOURCES        = aargb.c
aargb_LDFLAGS	     = ../../src/lib@LIBRARY@.la
checker_SOURCES      = checker.c
checker_LDFLAGS	     = ../../src/lib@LIBRARY@.la
feedback_SOURCES     = feedback.c
feedback_LDFLAGS     = ../../src/lib@LIBRARY@.la
list_SOURCES         = list.c
list_LDFLAGS	     = ../../src/lib@LIBRARY@.la
plane_SOURCES        = plane.c
plane_LDFLAGS	     = ../../src/lib@LIBRARY@.la
scene_SOURCES        = scene.c
scene_LDFLAGS	     = ../../src/lib@LIBRARY@.la
teapots_SOURCES      = teapots.c
teapots_LDFLAGS	     = ../../src/lib@LIBRARY@.la
torus_SOURCES        = torus.c
torus_LDFLAGS	     = ../../src/lib@LIBRARY@.la
accanti_SOURCES      = accanti.c
accanti_LDFLAGS	     = ../../src/lib@LIBRARY@.la
clip_SOURCES         = clip.c
clip_LDFLAGS	     = ../../src/lib@LIBRARY@.la
fog_SOURCES          = fog.c
fog_LDFLAGS	     = ../../src/lib@LIBRARY@.la
material_SOURCES     = material.c
material_LDFLAGS     = ../../src/lib@LIBRARY@.la
planet_SOURCES       = planet.c
planet_LDFLAGS	     = ../../src/lib@LIBRARY@.la
sceneflat_SOURCES    = sceneflat.c
sceneflat_LDFLAGS    = ../../src/lib@LIBRARY@.la
tess_SOURCES         = tess.c
tess_LDFLAGS	     = ../../src/lib@LIBRARY@.la
trim_SOURCES         = trim.c
trim_LDFLAGS	     = ../../src/lib@LIBRARY@.la
accpersp_SOURCES     = accpersp.c
accpersp_LDFLAGS     = ../../src/lib@LIBRARY@.la
colormat_SOURCES     = colormat.c
colormat_LDFLAGS     = ../../src/lib@LIBRARY@.la
fogindex_SOURCES     = fogindex.c
fogindex_LDFLAGS     = ../../src/lib@LIBRARY@.la
mipmap_SOURCES       = mipmap.c
mipmap_LDFLAGS	     = ../../src/lib@LIBRARY@.la
polyoff_SOURCES      = polyoff.c
polyoff_LDFLAGS	     = ../../src/lib@LIBRARY@.la
select_SOURCES       = select.c
select_LDFLAGS	     = ../../src/lib@LIBRARY@.la
tesswind_SOURCES     = tesswind.c
tesswind_LDFLAGS     = ../../src/lib@LIBRARY@.la
unproject_SOURCES    = unproject.c
unproject_LDFLAGS    = ../../src/lib@LIBRARY@.la
alpha3D_SOURCES      = alpha3D.c
alpha3D_LDFLAGS	     = ../../src/lib@LIBRARY@.la
cube_SOURCES         = cube.c
cube_LDFLAGS	     = ../../src/lib@LIBRARY@.la
font_SOURCES  	     = font.c
font_LDFLAGS	     = ../../src/lib@LIBRARY@.la
model_SOURCES        = model.c
model_LDFLAGS	     = ../../src/lib@LIBRARY@.la
polys_SOURCES        = polys.c
polys_LDFLAGS	     = ../../src/lib@LIBRARY@.la
smooth_SOURCES       = smooth.c
smooth_LDFLAGS	     = ../../src/lib@LIBRARY@.la
texbind_SOURCES      = texbind.c
texbind_LDFLAGS	     = ../../src/lib@LIBRARY@.la
varray_SOURCES       = varray.c
varray_LDFLAGS	     = ../../src/lib@LIBRARY@.la
alpha_SOURCES        = alpha.c
alpha_LDFLAGS	     = ../../src/lib@LIBRARY@.la
depthcue_SOURCES     = depthcue.c
depthcue_LDFLAGS     = ../../src/lib@LIBRARY@.la
hello_SOURCES        = hello.c
hello_LDFLAGS	     = ../../src/lib@LIBRARY@.la
movelight_SOURCES    = movelight.c
movelight_LDFLAGS    = ../../src/lib@LIBRARY@.la
quadric_SOURCES      = quadric.c
quadric_LDFLAGS	     = ../../src/lib@LIBRARY@.la
stencil_SOURCES      = stencil.c
stencil_LDFLAGS	     = ../../src/lib@LIBRARY@.la
texgen_SOURCES       = texgen.c
texgen_LDFLAGS	     = ../../src/lib@LIBRARY@.la
wrap_SOURCES         = wrap.c
wrap_LDFLAGS	     = ../../src/lib@LIBRARY@.la
anti_SOURCES         = anti.c
anti_LDFLAGS	     = ../../src/lib@LIBRARY@.la
dof_SOURCES          = dof.c
dof_LDFLAGS	     = ../../src/lib@LIBRARY@.la
image_SOURCES        = image.c
image_LDFLAGS	     = ../../src/lib@LIBRARY@.la
nurbs_SOURCES        = nurbs.c
nurbs_LDFLAGS	     = ../../src/lib@LIBRARY@.la
robot_SOURCES        = robot.c
robot_LDFLAGS	     = ../../src/lib@LIBRARY@.la
stroke_SOURCES       = stroke.c
stroke_LDFLAGS	     = ../../src/lib@LIBRARY@.la
texprox_SOURCES      = texprox.c
texprox_LDFLAGS	     = ../../src/lib@LIBRARY@.la
