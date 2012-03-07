
AUTOMAKE_OPTIONS = subdir-objects

ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = $(LIBJSON_CFLAGS)

bin_PROGRAMS = mustachec

mustachec_SOURCES = \
    mustachec.hpp \
    mustachec.cpp \
    jsonlint.h \
    jsonlint.c \
    ../src/mustache.hpp \
    ../src/mustache.cpp

mustachec_CFLAGS = -I$(srcdir)/../src
mustachec_CXXFLAGS = -I$(srcdir)/../src

mustachec_LDADD = $(LIBJSON_LIBS)

dist_noinst_SCRIPTS = autogen.sh
