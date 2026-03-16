#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_libarchive.h"

#ifndef HAVE_LIBARCHIVE_STREAM_CALLBACKS
#error "stream_callbacks.c compiled without HAVE_LIBARCHIVE_STREAM_CALLBACKS"
#endif

#include <php.h>
#include <archive.h>
#include <stdbool.h>
#include "php_libarchive.h"

#define STREAM_BLOCK_SIZE sizeof(((arch_stream_cb *)0)->buf)

static la_ssize_t stream_read_cb(struct archive *a, void *client_data,
                                 const void **buffer)
{
    arch_stream_cb *d = (arch_stream_cb *)client_data;
    php_stream *stream;
    size_t n;

    (void)a;
    php_stream_from_zval_no_verify(stream, &d->stream_zv);
    *buffer = d->buf;
    n = php_stream_read(stream, d->buf, STREAM_BLOCK_SIZE);
    return (la_ssize_t)n; /* 0 signals EOF to libarchive */
}

static la_int64_t stream_skip_cb(struct archive *a, void *client_data,
                                 la_int64_t request)
{
    arch_stream_cb *d = (arch_stream_cb *)client_data;
    php_stream *stream;

    (void)a;
    php_stream_from_zval_no_verify(stream, &d->stream_zv);
    if (php_stream_seek(stream, (zend_off_t)request, SEEK_CUR) == 0) {
        return request;
    }
    return 0; /* tell libarchive to use read-and-discard */
}

static la_int64_t stream_seek_cb(struct archive *a, void *client_data,
                                 la_int64_t offset, int whence)
{
    arch_stream_cb *d = (arch_stream_cb *)client_data;
    php_stream *stream;
    zend_off_t pos;

    (void)a;
    php_stream_from_zval_no_verify(stream, &d->stream_zv);
    if (php_stream_seek(stream, (zend_off_t)offset, whence) != 0) {
        return ARCHIVE_FAILED;
    }
    pos = php_stream_tell(stream);
    if (pos < 0) {
        return ARCHIVE_FAILED;
    }
    return (la_int64_t)pos;
}

static int stream_close_cb(struct archive *a, void *client_data)
{
    arch_stream_cb *d = (arch_stream_cb *)client_data;

    (void)a;
    zval_ptr_dtor(&d->stream_zv);
    efree(d);
    return ARCHIVE_OK;
}

bool arch_open_archive_with_stream_callbacks(arch_object *arch_obj)
{
    arch_stream_cb *d = arch_obj->source.stream_cb;

    archive_read_set_read_callback(arch_obj->archive, stream_read_cb);
    archive_read_set_skip_callback(arch_obj->archive, stream_skip_cb);
    archive_read_set_seek_callback(arch_obj->archive, stream_seek_cb);
    archive_read_set_close_callback(arch_obj->archive, stream_close_cb);
    archive_read_set_callback_data(arch_obj->archive, d);

    return archive_read_open1(arch_obj->archive) == ARCHIVE_OK;
}
