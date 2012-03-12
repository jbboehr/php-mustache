
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
    ../src/mustache.cpp \
    ../src/data.hpp \
    ../src/data.cpp \
    ../src/exception.hpp \
    ../src/node.hpp \
    ../src/node.cpp \
    ../src/renderer.hpp \
    ../src/renderer.cpp \
    ../src/tokenizer.hpp \
    ../src/tokenizer.cpp \
    ../src/utils.hpp \
    ../src/utils.cpp

mustachec_CFLAGS = -I$(srcdir)/../src
mustachec_CXXFLAGS = -I$(srcdir)/../src

mustachec_LDADD = $(LIBJSON_LIBS)

dist_noinst_SCRIPTS = autogen.sh
