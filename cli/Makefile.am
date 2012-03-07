
AUTOMAKE_OPTIONS = subdir-objects

ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = mustachec

mustachec_SOURCES = \
    mustachec.hpp \
    mustachec.cpp \
    ../src/mustache.hpp \
    ../src/mustache.cpp

mustachec_CFLAGS = -I$(srcdir)/../src
mustachec_CXXFLAGS = -I$(srcdir)/../src

dist_noinst_SCRIPTS = autogen.sh
