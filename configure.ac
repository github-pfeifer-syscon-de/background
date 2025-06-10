AC_INIT([background program],[0.1],[background@pfeifer-syscon.de])

AC_CONFIG_MACRO_DIR([m4])

dnl use -no-portablitity to stop complaining about $(shell... in res Makefile.am
dnl i dont get it, as the shell is everywhere, turtle power!
dnl alternative use flavor foreign

AM_INIT_AUTOMAKE([1.11 -Wno-portability])

AM_SILENT_RULES([yes])

AC_CONFIG_HEADERS([config.h])

AX_CHECK_ENABLE_DEBUG([info], [DEBUG], [RELEASE], [IS-RELEASE])
AC_PROG_CC
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([20], [noext], [optional])

LT_INIT

AC_CANONICAL_HOST
AC_MSG_NOTICE([host_os $host_os])

PKG_CHECK_MODULES([GENERICIMG], [genericimg >= 0.4.3])

dnl dont repeat dependies from genericimg
dnl PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0])
dnl PKG_CHECK_MODULES([GLIBMM], [glibmm-2.4 giomm-2.4])
dnl PKG_CHECK_MODULES(JSONGLIB1, [json-glib-1.0])

AC_ARG_WITH([python], AS_HELP_STRING([--without-python], [If you don't like the python info scripting option (internal functions will will be used instead)]))

AS_IF([test "x$with_python" != "xno"], [
    PKG_CHECK_MODULES([PYTHON], [python3-embed py3cairo pygobject-3.0], [AC_DEFINE([USE_PYTHON], [1], [Using python])])
])

AM_CONDITIONAL([PYTHON], [test "x$with_python" != "xno"])


AC_ARG_WITH([appmenu], AS_HELP_STRING([--with-appmenu], [Include a app-menu EXPERIMENTAL (based on garcon2, comes with xfce4)]))

AS_IF([test "x$with_appmenu" == "xyes"], [
    PKG_CHECK_MODULES([GARCON], [garcon-gtk3-1], [AC_DEFINE([USE_APPMENU], [1], [Using python])])
])

AM_CONDITIONAL([APPMENU], [test "x$with_appmenu" == "xyes"])


AC_SEARCH_LIBS([HPDF_Page_BeginText], [hpdf], [USE_LIBHARU="true"])

AM_CONDITIONAL([HAVE_PDF], [test x$USE_LIBHARU = xtrue])

if test "x$USE_LIBHARU" = xtrue; then
    AC_DEFINE([USE_PDF], [1], [Using hpdf])
fi

if test "$host_os" = "mingw32" ; then
    AC_SUBST(EXTRA_LDFLAGS, "-mwindows")
fi

AC_CONFIG_FILES([
Makefile
test/Makefile
res/Makefile
src/Makefile])
AC_OUTPUT
