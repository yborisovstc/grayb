
AC_INIT([grayb], [0.1], [yuri.borisov.v@gmail.com])

AM_INIT_AUTOMAKE([-Wall])

AC_CONFIG_FILES([
		 Makefile
		 src/Makefile
		 test/Makefile
		 ])


AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

# Configure options: --enable-debug[=no].
AC_ARG_ENABLE([debug],
	      [AS_HELP_STRING([--enable-debug],
			      [enable debug code (default is no)])],
			      [debug="$withval"], [debug=no])

PKG_CHECK_MODULES(LIBXML, [libxml-2.0], , [AC_MSG_ERROR([
You must have the libxml-2.0 and development headers installed to build.
])])

AC_SUBST(LIBXML_CFLAGS)
AC_SUBST(LIBXML_LIBS)

if test x$debug = xyes; then
    CXXFLAGS="-g -O0"
else
#    CXXFLAGS="-g -O2"
    CXXFLAGS="-g -O0"
fi


CXXFLAGS="$CXXFLAGS $GLIB_CFLAGS $LIBXML_CFLAGS" 
LDFLAGS="$LDFLAGS $GLIB_LIBS $LIBXML_LIBS"

AC_CONFIG_FILES([libgrayb.pc])

AC_OUTPUT

echo CXXFLAGS $CXXFLAGS
echo "
($PACKAGE_NAME) version $PACKAGE_VERSION
Prefix.........: $prefix
Debug Build....: $debug
C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS
Linker.........: $LD $LDFLAGS $LIBS
Doxygen........: ${DOXYGEN:-NONE}
"

