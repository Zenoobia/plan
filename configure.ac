# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([plan], [1.0], [joaqimpla@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main_new.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Conditionals
AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug], [enable debugging, default: yes]),
[case "${enableval}" in
      yes) debug=true ;;
      no)  debug=false ;;
      *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

# Checks for libraries.
LT_INIT
AC_ENABLE_SHARED
AC_DISABLE_STATIC
AC_PROG_LIBTOOL(libtool)

AC_CHECK_LIB(glfw, glfwInit, [], AC_MSG_ERROR([Could not find libglfw. Try $ ./configure LDFLAGS='-Lyour-glfw-lib-path']))
#AC_CHECK_LIB(GL, glPushMatrix, [], AC_MSG_ERROR([Could not find libGL. Try $ ./configure LDFLAGS='-Lyour-GL-lib-path']))
AC_CHECK_LIB(opengl32, glPushMatrix, [], AC_MSG_ERROR([Could not find libGL. Try $ ./configure LDFLAGS='-Lyour-GL-lib-path']))

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdint.h string.h sys/time.h termios.h unistd.h])
#AC_CHECK_HEADERS([argtable2.h], [], AC_MSG_ERROR([Could not find argtable2.h. Try $ ./configure CFLAGS='-Iyour-argtable2-include-path]))
AC_CHECK_HEADERS([GLFW/glfw3.h], [], AC_MSG_ERROR([Could not find GLFW/glfw3.h. Try $ ./configure CFLAGS='-Iyour-GLFW-include-path]))

# Finds Dependencies
PKG_CHECK_MODULES([DEPS], [glib-2.0 >= 2.24.1] [opencv] [glew] [glfw3] [lept] [tesseract] [poppler-cpp])
#-lglfw3 -pthread -ldl -lGLU -lGL -lrt -lXrandr -lXxf86vm -lXi -lXinerama -lX11
#PKG_CHECK_MODULES([DEPS], [opencv-3.4.1-2 >= 3])
#PKG_CHECK_MODULES([DEPS], [glew-2.1.0])
#PKG_CHECK_MODULES([DEPS], [glut-3.0.0])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Output Makefile files.
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile examples/Makefile man/Makefile scripts/Makefile ])
# third_party/repos/imgui/Makefile \
AC_OUTPUT

