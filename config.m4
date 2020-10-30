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

  PHP_SUBST(ARCHIVE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(archive, libarchive.c stream.c, $ext_shared,,-fvisibility=hidden)
fi
