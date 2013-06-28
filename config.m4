
dnl FUNCTIONS ------------------------------------------------------------------
AC_DEFUN([PHP_MUSTACHE_ADD_SOURCES], [
  PHP_MUSTACHE_SOURCES="$PHP_MUSTACHE_SOURCES $1"
])
AC_DEFUN([PHP_MUSTACHE_ADD_FLAGS], [
  PHP_MUSTACHE_FLAGS="$PHP_MUSTACHE_FLAGS $1"
])
AU_ALIAS([AC_CXX_COMPILE_STDCXX_0X], [AX_CXX_COMPILE_STDCXX_0X])
AC_DEFUN([AX_CXX_COMPILE_STDCXX_0X], [
  AC_CACHE_CHECK(if g++ supports C++0x features without additional flags,
  ax_cv_cxx_compile_cxx0x_native,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([
  template <typename T>
    struct check
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };

    typedef check<check<bool>> right_angle_brackets;

    int a;
    decltype(a) b;

    typedef check<int> check_type;
    check_type c;
    check_type&& cr = static_cast<check_type&&>(c);],,
  ax_cv_cxx_compile_cxx0x_native=yes, ax_cv_cxx_compile_cxx0x_native=no)
  AC_LANG_RESTORE
  ])

  AC_CACHE_CHECK(if g++ supports C++0x features with -std=c++0x,
  ax_cv_cxx_compile_cxx0x_cxx,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=c++0x"
  AC_TRY_COMPILE([
  template <typename T>
    struct check
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };

    typedef check<check<bool>> right_angle_brackets;

    int a;
    decltype(a) b;

    typedef check<int> check_type;
    check_type c;
    check_type&& cr = static_cast<check_type&&>(c);],,
  ax_cv_cxx_compile_cxx0x_cxx=yes, ax_cv_cxx_compile_cxx0x_cxx=no)
  CXXFLAGS="$ac_save_CXXFLAGS"
  AC_LANG_RESTORE
  ])

  AC_CACHE_CHECK(if g++ supports C++0x features with -std=gnu++0x,
  ax_cv_cxx_compile_cxx0x_gxx,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=gnu++0x"
  AC_TRY_COMPILE([
  template <typename T>
    struct check
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };

    typedef check<check<bool>> right_angle_brackets;

    int a;
    decltype(a) b;

    typedef check<int> check_type;
    check_type c;
    check_type&& cr = static_cast<check_type&&>(c);],,
  ax_cv_cxx_compile_cxx0x_gxx=yes, ax_cv_cxx_compile_cxx0x_gxx=no)
  CXXFLAGS="$ac_save_CXXFLAGS"
  AC_LANG_RESTORE
  ])

  if test "$ax_cv_cxx_compile_cxx0x_native" = yes ||
     test "$ax_cv_cxx_compile_cxx0x_cxx" = yes ||
     test "$ax_cv_cxx_compile_cxx0x_gxx" = yes; then
    AC_DEFINE(HAVE_STDCXX_0X,,[Define if g++ supports C++0x features. ])
    HAVE_STDCXX_0X="yes"
  fi
])

dnl C++0X ----------------------------------------------------------------------
AX_CXX_COMPILE_STDCXX_0X()

dnl MUSTACHE -------------------------------------------------------------------
PHP_ARG_ENABLE(mustache, whether to enable mustache support,
dnl Make sure that the comment is aligned:
[  --enable-mustache Enable mustache support])

dnl TCMALLOC -------------------------------------------------------------------
AC_MSG_CHECKING(whether tcmalloc support is present and requested)
AC_ARG_ENABLE(mustache-tcmalloc,
[  --enable-mustache-tcmalloc     Enable mustache tcmalloc], 
[
  PHP_MUSTACHE_TCMALLOC=$enableval
  AC_MSG_RESULT($enableval)
], 
[
  PHP_MUSTACHE_TCMALLOC=no
  AC_MSG_RESULT(no)
])

if test "$PHP_MUSTACHE_TCMALLOC" == "yes"; then
  PHP_ADD_LIBRARY(tcmalloc, 1, MUSTACHE_SHARED_LIBADD)
  AC_DEFINE(HAVE_TCMALLOC, 1, [Whether tcmalloc support is present and requested])
fi

dnl BOOST ----------------------------------------------------------------------
AC_MSG_CHECKING(whether libboost support is present and requested)
AC_ARG_ENABLE(mustache-libboost,
[  --enable-mustache-libboost     Enable mustache libboost], 
[
  PHP_MUSTACHE_LIBBOOST=$enableval
  AC_MSG_RESULT($enableval)
], 
[
  PHP_MUSTACHE_LIBBOOST=no
  AC_MSG_RESULT(no)
])

if test "$PHP_MUSTACHE_LIBBOOST" == "yes"; then
  PHP_ADD_LIBRARY(boost_serialization, 1, MUSTACHE_SHARED_LIBADD)
  AC_DEFINE(HAVE_LIBBOOST, 1, [Whether libboost support is present and requested])
fi

dnl MAIN -----------------------------------------------------------------------
if test "$PHP_MUSTACHE" != "no"; then
  
  AC_DEFINE(HAVE_MUSTACHE, 1, [Whether you have mustache support])
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, MUSTACHE_SHARED_LIBADD)
  
  dnl if test "$ax_cv_cxx_compile_cxx0x_cxx" = yes; then
  dnl   PHP_MUSTACHE_ADD_FLAGS("-std=c++0x")
  dnl elif test "$ax_cv_cxx_compile_cxx0x_gxx" = yes; then
  dnl   PHP_MUSTACHE_ADD_FLAGS("-std=gnu++0x")
  dnl fi

  PHP_ADD_INCLUDE(mustache/src)
  PHP_ADD_BUILD_DIR(mustache/src)
  PHP_MUSTACHE_ADD_SOURCES([
    php_mustache.cpp
    mustache_mustache.cpp
    mustache_exceptions.cpp
    mustache_data.cpp
    mustache_template.cpp
    mustache/src/mustache.cpp
    mustache/src/data.cpp
    mustache/src/node.cpp
    mustache/src/renderer.cpp
    mustache/src/tokenizer.cpp
    mustache/src/utils.cpp
  ])
  
  PHP_SUBST(MUSTACHE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mustache, $PHP_MUSTACHE_SOURCES, $ext_shared, , $PHP_MUSTACHE_FLAGS)
fi
