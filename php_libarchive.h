#ifndef PHP_LIBARCHIVE_H
# define PHP_LIBARCHIVE_H

#include <php.h>

/* libarchive extension for PHP */

# define PHP_LIBARCHIVE_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_LIBARCHIVE)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#ifndef __has_feature
# define __has_feature(x) 0
#endif

#if !__has_feature(nullability)
# ifndef _Nullable
#  define _Nullable
# endif
# ifndef _Nonnull
#  define _Nonnull
# endif
# ifndef _Null_unspecified
#  define _Null_unspecified
# endif
#endif

#define nonnull _Nonnull
#define nullable _Nullable
#define unspecnull _Null_unspecified

typedef struct _arch_object {
    zend_string *nullable file_location;
    struct archive *nullable archive;
    struct archive *nullable arch_disk;
    int write_disk_options;
    zend_object parent;
} arch_object;

static inline arch_object *arch_object_fetch(zend_object *zobj)
{
    return (arch_object *)(uintptr_t)((char *)zobj - XtOffsetOf(arch_object, parent));
}

static inline arch_object *arch_object_from_zv(const zval *zv)
{
    return arch_object_fetch(Z_OBJ_P(zv));
}

#endif	/* PHP_LIBARCHIVE_H */
