#! /bin/bash

# Clean out all of the cruft created when running autotools
if [ -e Makefile ]; then
    make clean
fi

rm -rf  \
   Makefile.in \
   install-sh \
   config.h.in \
   configure \
   depcomp \
   INSTALL \
   compile \
   missing \
   autom4te.cache \
   aclocal.m4 \
   AUTHORS \
   NEWS \
   README \
   COPYING \
   .deps \
   Makefile \
   config.status \
   config.h \
   config.log \
   stamp-h1
