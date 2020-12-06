PHP_ARG_WITH([libarchive],
  [for libarchive support],
  [AS_HELP_STRING([[--with-libarchive[=DIR]]],
    [Include libarchive support])])

if test "$PHP_LIBARCHIVE" != "no"; then
  if test -r $PHP_LIBARCHIVE/include/archive.h; then
    LIBARCHIVE_DIR=$PHP_LIBARCHIVE
  else
    AC_MSG_CHECKING(for libarchive in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/archive.h; then
        LIBARCHIVE_DIR=$i
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
  fi

  if test -z "$LIBARCHIVE_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please install libarchive first)
  fi

  PHP_ADD_INCLUDE($LIBARCHIVE_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH(archive, $LIBARCHIVE_DIR/$PHP_LIBDIR, ARCHIVE_SHARED_LIBADD)

  PHP_CHECK_LIBRARY(archive, archive_version_number,
  [
    AC_DEFINE(HAVE_LIBARCHIVE,1,[ ])
  ], [
    AC_MSG_ERROR(function archive_version_number not found)
  ], [
    -L$LIBARCHIVE_DIR/$PHP_LIBDIR
  ])

  cflags_null=""
  AC_DEFUN([CC_FLAG_CHECK],
  [
  ac_saved_cflags="$CFLAGS"
  CFLAGS="$1 -Werror"
  flag_to_add=m4_default([$2],[$1])
  AC_MSG_CHECKING([whether the C compiler supports $1])
  AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([])],
    [AC_MSG_RESULT([yes])]
    [cflags_null="$cflags_null $flag_to_add"],
    [AC_MSG_RESULT([no])]
  )
  CFLAGS="$ac_saved_cflags"
  ])
  CC_FLAG_CHECK([-Wnullability-completeness], [-Wno-nullability-completeness])

  extra_cflags="$cflags_null -fvisibility=hidden"
  echo "EXTRA_CFLAGS := \$(EXTRA_CFLAGS) $extra_cflags" >> Makefile.fragments

  PHP_SUBST(ARCHIVE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(archive, libarchive.c stream.c, $ext_shared,,-Wall)
fi
