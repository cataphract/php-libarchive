#ifndef PHP_LIBARCHIVE_H
#define PHP_LIBARCHIVE_H

#include <php.h>
#include <archive.h>

/* libarchive extension for PHP */

#define PHP_LIBARCHIVE_VERSION "0.2.0"

#if defined(ZTS) && defined(COMPILE_DL_LIBARCHIVE)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#if !__has_feature(nullability)
#ifndef _Nullable
#define _Nullable
#endif
#ifndef _Nonnull
#define _Nonnull
#endif
#ifndef _Null_unspecified
#define _Null_unspecified
#endif
#endif

#define nonnull _Nonnull
#define nullable _Nullable
#define unspecnull _Null_unspecified

typedef enum {
    ARCH_SOURCE_NONE,
    ARCH_SOURCE_FILE,
    ARCH_SOURCE_STREAM,
} arch_source_kind;

typedef struct _arch_object {
    arch_source_kind source_kind;
    union {
        zend_string *file_location; /* ARCH_SOURCE_FILE */
        zval stream_zv;             /* ARCH_SOURCE_STREAM */
    } source;
    struct archive *nullable archive;
    struct archive *nullable arch_disk;
    int write_disk_options;
    int *nullable formats; /* NULL = auto-detect all; emalloc'd array of ARCHIVE_FORMAT_* codes */
    uint32_t formats_count;
    int *nullable filters; /* NULL = auto-detect all; emalloc'd array of ARCHIVE_FILTER_* codes */
    uint32_t filters_count;
    uint32_t entry_generation; /* incremented before each archive_read_next_header2 call */
    la_int64_t current_entry_size; /* archive_entry_size() of the current entry, or -1 if not set */
    zend_fcall_info password_fci;               /* size == 0 if not set */
    zend_fcall_info_cache password_fcc;
    zend_string *nullable password_cb_last_result; /* owns the last passphrase string */
    zend_object parent;
} arch_object;

static inline arch_object *arch_object_fetch(zend_object *zobj)
{
    return (arch_object *)(uintptr_t)((char *)zobj -
                                      XtOffsetOf(arch_object, parent));
}

static inline arch_object *arch_object_from_zv(const zval *zv)
{
    return arch_object_fetch(Z_OBJ_P(zv));
}

#endif /* PHP_LIBARCHIVE_H */
