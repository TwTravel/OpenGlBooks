#!/bin/sh
# Copyright 2004, the OpenGLUT contributors.
#
# Invocation:
#    sh release.sh PRIME MAJOR [MINOR [RC]]
#
#    ...where the parameters are all numbers.
#    The fullname of a major release is:
#
#      OpenGLUT-PRIME-MAJOR
#
#    A minor update is:
#
#      OpenGLUT-PRIME-MAJOR-MINOR
#
#    Major releases and minor updates can have Release Candidates,
#    in each case denoted by a "_rc${RC}" string.
#
#    A MAJOR release can be noted either by not specifying
#    a MINOR, or else by setting MINOR to 0.
#
# Things to do for a Release:
#
#   Determine Prime, Major, and Minor version number + RC iteration.
#   Full name: OpenGLUT-PRIME-MAJOR-MINOR[_RC]
#              where [_RC] is omitted if not a release candidate.
#              Write #s for the numbers (if only 3 #s then no RC)
#              Below, {ver} is the version, with optionan numbers
#              numbers omitted gracefully.
#              libtool version components are lib_current, lib_revision,
#              and lib_age, and are represented in a tuple, below, as
#              L:L:L (since # is a pound-sign...(^&)
#   Edit:
#     ./configure.in               AM_INIT_AUTOMAKE(openglut.#.#.#)
#     ./doc/doxygen.cfg            PROJECT_NUMBER         = "{ver} development"
#     ./openglut.lsm               Version:        #.#.#
#     ./include/GL/openglut_std.h  #define OPENGLUT_VERSION_#_#_#
#     ./src/Makefile.am            libopenglut_la_LDFLAGS = -version-info L:L:L
#     ./src/og_internal.h          #define VERSION_MAJOR <prime>
#                                  #define VERSION_MINOR <major>
#                                  #define VERSION_PATCH <minor>
#     ./src/openglut.def           VERSION #.#
#   CVS commit (optional)
#   CVS tag    (optional)
#   Add a line to ChangeLog, perhaps.
#

# Load the release configuration file.
#
# (Note that not all parts of it are currently used.)
#
. `pwd`/release.conf

# These operations are for more automated versioning---not yet working
# to my satisfaction.
#
#cmds=0
#for cmd
#do
#  cmds=$(($cmds+1))
#  case $cmd in
#    freeze)
#      if [ 0 = $rc ]; then
#        patch=$(($patch+1))
#      else
#         rc=0
#      fi;;
#    rc)
#      rc=$(($rc+1));;
#  esac
#done
#if [ 0 = $cmds ]; then
#  if [ 0 = $rc ]; then
#    patch=$(($patch+1))
#  else
#    rc=$(($rc+1))
#  fi
#fi
#echo $major
#echo $minor
#echo $patch
#echo $rc
#exit

minor_set=0
rc_set=0
major_release=1
release_candidate=0
prime=${1?Must set a primary release number.}
major=${2?Must set a major version number.}
minor=$3
rc=$4

# $fullname is the CVS tag: OpenGLUT-#-#[-#[_#]].
# $fullvers uses "." instead of "-" for separators, and is
#   just the (project) version.
# $tarname is the root for the dist. tarball:
#   openglut-#.#.# (no rc; always include the $minor)
#
fullname='OpenGLUT-'${prime}'-'${major}
fullvers=${prime}'.'${major}
tarname="openglut-${fullvers}"
if test 'x' = ${minor}'x'
then
    minor=0
    tarname=${tarname}'.0'
else
    minor_set=1
    if test 0 != ${minor}
    then
	major_release=0
	minor_or_blank=${minor}
	fullname=${fullname}'-'${minor}
	fullvers=${fullvers}'.'${minor}
    fi
    tarname=${tarname}'.'${minor}
    
    if test 'x' = ${rc}'x'
    then
	printf 'A real release.  Not a release-candidate.\n'
    else
	rc_set=1
	release_candidate=1
	fullname=${fullname}'_rc'${rc}
	fullvers=${fullvers}'_rc'${rc}
    fi
fi


printf "\n"
printf "Automatically editing:\n"

printf "  ./configure.in\n"
printf '/AM_INIT_AUTOMAKE/s/^.*$/AM_INIT_AUTOMAKE\\(openglut,%d\\.%d\\.%d\\)/\n' ${prime} ${major} ${minor} >sed_cmd
sed <./configure.in -f sed_cmd >sed_out
mv sed_out ./configure.in

printf "  ./doc/doxygen.cfg:\n"
printf '/^PROJECT_NUMBER/s/^.*$/PROJECT_NUMBER         = "%s development"/\n' ${fullvers} >sed_cmd
sed <./doc/doxygen.cfg -f sed_cmd >sed_out
mv sed_out ./doc/doxygen.cfg

printf "  ./include/GL/openglut_std.h\n"
printf '/#define OPENGLUT_VERSION/s/^.*$/#define OPENGLUT_VERSION_%d_%d_%d/\n' ${prime} ${major} ${minor} >sed_cmd
sed <./include/GL/openglut_std.h -f sed_cmd >sed_out
mv sed_out ./include/GL/openglut_std.h

printf "  ./openglut.lsm\n"
printf '/^Version\\:/s/^.*$/Version:        %d\\.%d\\.%d/\n' ${prime} ${major} ${minor} >sed_cmd
sed <./openglut.lsm -f sed_cmd >sed_out
mv sed_out ./openglut.lsm

printf "  ./src/og_internal.h\n"
printf '/^#define VERSION_MAJOR/s/^.*$/#define VERSION_MAJOR %d/\n' ${prime} >sed_cmd
printf '/^#define VERSION_MINOR/s/^.*$/#define VERSION_MINOR %d/\n' ${major} >>sed_cmd
printf '/^#define VERSION_PATCH/s/^.*$/#define VERSION_PATCH %d/\n' ${minor} >>sed_cmd
sed <./src/og_internal.h -f sed_cmd >sed_out
mv sed_out ./src/og_internal.h

printf "  ./src/Makefile.am\n"
printf '/libopenglut_la_LDFLAGS/s/^.*$/libopenglut_la_LDFLAGS = -version-info %s:%s:%s/\n' ${lib_current} ${lib_revision} ${lib_age} >sed_cmd
sed  <./src/Makefile.am -f sed_cmd >sed_out
mv sed_out ./src/Makefile.am

printf "  ./src/openglut.def\n"
printf '/VERSION/s/^.*$/VERSION %d\\.%d/\n' ${prime} ${major} >sed_cmd
sed <./src/openglut.def -f sed_cmd >sed_out
mv sed_out ./src/openglut.def
rm sed_cmd

printf "\n"
printf "We have edited all of the version information (finters crossed),\n"
printf "and are now ready to optionally commit and tag (%s)\n" "$fullname"
printf "to the CVS repository.  Then, to build the tarballs we must re-run\n"
printf "the configure process (this ensures that the tarballs build with\n"
printf "the new version numbers).  To make life simpler, some common\n"
printf "./configure options are allowed.  See release.conf and release.local.\n"
printf "\n"
printf "Note that we have to run ./configure twice to get around a feature\n"
printf "of the GNU auto* tools that causes it to incorrectly regenerate our\n"
printf "config.h file in some cases.  (There may be a better way, but the GNU\n"
printf "auto* tools are not really something that I know much about, nor do I\n"
printf "have time for digging into them right now.)  The second run is so\n"
printf "your environment is okay for building OpenGLUT binaries.\n"
printf "\n"

printf "Commit?  [No]  "
read commit
commit=`echo $commit | tr A-Z a-z`
commit=${commit%%es}
if test 'xy' = x$commit
then
  cvs commit
fi
printf "fullname: %s\n" ${fullname}
printf "Tag it?  [No]  "
read tag_it
tag_it=`echo $tag_it | tr A-Z a-z`
tag_it=${tag_it%%es}
if test 'xy' = x$tag_it
then
  printf "%s\n" "--- Tagging via:"
  printf "cvs tag ${fullname}\n"
  cvs tag "${fullname}"
fi

# Now, we've edited and perhaps committed.  Let's configure a
# local build.
#
rm ${tarname}*.tar*
sh autogen.sh
sleep 1
./configure

archives='openglut-*.tar.gz openglut-*.tar.bz2'
rm -f ${archives}
make dist-bzip2
cat ${tarname}.tar.bz2 | bunzip2 | gzip -9 >${tarname}.tar.gz
(cd doc && make documentation)
tar -cvf ${tarname}-doc.tar doc/cat3 doc/html doc/man doc/ps
cat ${tarname}-doc.tar | bzip2 -9 >${tarname}-doc.tar.bz2
gzip -9 ${tarname}-doc.tar
(make clean && cd doc && make clean)
rm -f fingerprints
cksum >>fingerprints ${archives}
sum >>fingerprints ${archives}
md2 >>fingerprints ${archives}
md4 >>fingerprints ${archives}
md5 >>fingerprints ${archives}
sha1 >>fingerprints ${archives}
rmd160 >>fingerprints ${archives}



# Now, restore the ``normal'' build environment, using
# release.local's definitions to guide us.
#
# See ./release.conf for details about the variables here.
# Set these variables in release.local if you like.
#
LDFLAGS="${ldflags}" ./configure ${conf_args}

