#include "php_libarchive.h"
#include "zend_variables.h"
#include <archive.h>

#if PHP_VERSION_ID < 70400
typedef size_t stream_ret_t;
#define STREAM_ERR_RET 0
#else
typedef ssize_t stream_ret_t;
#define STREAM_ERR_RET 1
#endif

static stream_ret_t php_arch_ops_write(php_stream *stream,
                                       const char *buf,
                                       size_t count)
{
    (void)stream;
    (void)buf;
    (void)count;
    php_error_docref(NULL, E_WARNING,
                    "Write operation not supported for libarchive streams.");
    return STREAM_ERR_RET;
}

static inline struct archive *nullable arch_from_stream(php_stream *stream)
{
    zval *arch_zv = stream->abstract;

    if (!arch_zv) {
        php_error_docref(NULL, E_WARNING, "Stream already closed");
        return NULL;
    }

    arch_object *obj = arch_object_from_zv(arch_zv);
    struct archive *arch = obj->archive;
    if (!arch) {
        php_error_docref(NULL, E_WARNING,
                         "libarchive handle has been disposed of already");
    }
    return arch;
}

static stream_ret_t php_arch_ops_read(php_stream *stream, char *buf, size_t count)
{
    struct archive *arch = arch_from_stream(stream);
    if (!arch) {
        return STREAM_ERR_RET;
    }

    if (UNEXPECTED(count == 0)) {
        return 0;
    }

    la_ssize_t read = archive_read_data(arch, buf, count);
    if (read < 0) {
        php_error_docref(NULL, E_WARNING, "Error reading data: %s [%d]",
                         archive_error_string(arch), archive_errno(arch));
        return STREAM_ERR_RET;
    }

    return (stream_ret_t)read;
}

static int php_arch_ops_close(php_stream *stream, int close_handle)
{
    if (!close_handle) {
        // should only happen if there was a cast
        php_error_docref(NULL, E_WARNING, "Preserving handle not supported");
    }
    zval *arch = stream->abstract;
    if (!arch) {
        return EOF;
    }
    zval_ptr_dtor(arch);
    stream->abstract = NULL;
    return EOF;
}

static int php_arch_ops_flush(php_stream *stream)
{
    (void)stream;
    return 0;
}

static int php_arch_ops_seek(php_stream *stream, zend_off_t offset,
                             int whence, zend_off_t *newoffset)
{
    struct archive *arch = arch_from_stream(stream);
    if (!arch) {
        return -1;
    }

    la_int64_t res = archive_seek_data(arch, offset, whence);
    if (res < 0) {
        php_error_docref(NULL, E_WARNING, "Error seeking in entry: %s [%d]",
                         archive_error_string(arch), archive_errno(arch));
        return -1;
    }

    *newoffset = (zend_off_t)res;
    return 0; // SUCCESS
}

static php_stream_ops php_stream_arch_ops = {
    .write = php_arch_ops_write,
    .read = php_arch_ops_read,
    .close = php_arch_ops_close,
    .flush = php_arch_ops_flush,
    .label = "libarchive",
    .seek = php_arch_ops_seek,
};

// TODO: stream should be invalidated once we move to the next entry
php_stream *php_stream_arch_cur_entry_stream(zval *nonnull arch_zv)
{
    zval_add_ref(arch_zv);
    php_stream *stream = php_stream_alloc(
        &php_stream_arch_ops, arch_zv, NULL, "rb");
    return stream;
}
