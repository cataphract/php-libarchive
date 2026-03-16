#ifndef PHP_LIBARCHIVE_H
#define PHP_LIBARCHIVE_H

#include <php.h>
#include <archive.h>

/* libarchive extension for PHP */

#define PHP_LIBARCHIVE_VERSION "0.3.0-dev"

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

/* On Windows, where fopencookie and similar is unavailable, the callback path
 * is always available; on other platforms it is enabled when the extension was
 * built with --with-libarchive-read-callbacks. */
#if defined(PHP_WIN32) && !defined(HAVE_LIBARCHIVE_STREAM_CALLBACKS)
#define HAVE_LIBARCHIVE_STREAM_CALLBACKS 1
#endif

typedef enum {
    ARCH_SOURCE_NONE,
    ARCH_SOURCE_FILE,
#ifndef HAVE_LIBARCHIVE_STREAM_CALLBACKS
    ARCH_SOURCE_STREAM,
#else
    ARCH_SOURCE_STREAM_CB,
#endif
} arch_source_kind;

#ifdef HAVE_LIBARCHIVE_STREAM_CALLBACKS
typedef struct {
    zval stream_zv;
    char buf[65536];
} arch_stream_cb;
#endif

typedef struct _arch_object {
    arch_source_kind source_kind;
    union {
        zend_string    *file_location; /* ARCH_SOURCE_FILE */
#ifndef HAVE_LIBARCHIVE_STREAM_CALLBACKS
        zval            stream_zv;     /* ARCH_SOURCE_STREAM */
#else
        arch_stream_cb *stream_cb;     /* ARCH_SOURCE_STREAM_CB */
#endif
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
