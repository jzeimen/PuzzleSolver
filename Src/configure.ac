AC_INIT([Puzzle Solver], 2019.1)
AC_PREREQ(2.5)
 
AM_INIT_AUTOMAKE(1.1)
 
# This is a c++ project
AC_PROG_CXX
 
# This will create a header containing some important compiler directives
AC_CONFIG_HEADERS([config.h])
 
# This configures openmp
AC_OPENMP
# In mac omp can be used through intel's iomp5. 
AC_CHECK_LIB(gomp, omp_get_thread_num, , \
    AC_CHECK_LIB(iomp5, omp_get_thread_num))
AC_CHECK_HEADERS([omp.h])

PKG_CHECK_MODULES(opencv4, [opencv4], [], [PKG_CHECK_MODULES(opencv, [opencv], [], [AC_MSG_ERROR("opencv not found")])])

AM_CONDITIONAL([USE_OPENCV4], [test "${opencv4_CFLAGS}"])

AC_SUBST(major)
AC_SUBST(minor)
 
# This tells configure.ac which make files to use.
AC_OUTPUT(Makefile)

