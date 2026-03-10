#include "php_libarchive.h"
#include "zend_variables.h"
#include <archive.h>

typedef ssize_t stream_ret_t;
#define STREAM_ERR_RET ((ssize_t) - 1)

typedef struct {
    zval arch_zv;
    uint32_t generation;
    la_int64_t remaining_bytes; /* bytes left to read, or -1 if entry size is not set */
} arch_stream_data;

static stream_ret_t php_arch_ops_write(php_stream *stream, const char *buf,
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
    arch_stream_data *data = stream->abstract;

    if (!data) {
        php_error_docref(NULL, E_WARNING, "Stream already closed");
        return NULL;
    }

    arch_object *obj = arch_object_from_zv(&data->arch_zv);
    if (obj->entry_generation != data->generation) {
        php_error_docref(NULL, E_WARNING,
                         "Stream is no longer valid: the archive has moved to "
                         "the next entry");
        return NULL;
    }

    struct archive *arch = obj->archive;
    if (!arch) {
        php_error_docref(NULL, E_WARNING,
                         "libarchive handle has been disposed of already");
    }
    return arch;
}

static stream_ret_t php_arch_ops_read(php_stream *stream, char *buf,
                                      size_t count)
{
    struct archive *arch = arch_from_stream(stream);
    if (!arch) {
        return STREAM_ERR_RET;
    }

    if (UNEXPECTED(count == 0)) {
        return 0;
    }

    // Cap reads at the declared entry size
    arch_stream_data *data = stream->abstract;
    if (data->remaining_bytes >= 0) {
        if (data->remaining_bytes == 0) {
            return 0; /* EOF */
        }
        if ((la_int64_t)count > data->remaining_bytes) {
            count = (size_t)data->remaining_bytes;
        }
    }

    la_ssize_t read = archive_read_data(arch, buf, count);
    if (read < 0) {
        php_error_docref(NULL, E_WARNING, "Error reading data: %s [%d]",
                         archive_error_string(arch), archive_errno(arch));
        return STREAM_ERR_RET;
    }

    if (data->remaining_bytes >= 0) {
        data->remaining_bytes -= read;
    }

    return (stream_ret_t)read;
}

static int php_arch_ops_close(php_stream *stream, int close_handle)
{
    if (!close_handle) {
        // should only happen if there was a cast
        php_error_docref(NULL, E_WARNING, "Preserving handle not supported");
    }
    arch_stream_data *data = stream->abstract;
    if (!data) {
        return EOF;
    }
    zval_ptr_dtor(&data->arch_zv);
    efree(data);
    stream->abstract = NULL;
    return EOF;
}

static int php_arch_ops_flush(php_stream *stream)
{
    (void)stream;
    return 0;
}

static int php_arch_ops_seek(php_stream *stream, zend_off_t offset, int whence,
                             zend_off_t *newoffset)
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
	arch_stream_data *data = stream->abstract;
	if (data->remaining_bytes >= 0) {
		la_int64_t entry_size = arch_object_from_zv(&data->arch_zv)->current_entry_size;
		if (entry_size >= 0) {
			data->remaining_bytes = entry_size - res;
		}
	}
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

php_stream *php_stream_arch_cur_entry_stream(zval *nonnull arch_zv)
{
    arch_stream_data *data = emalloc(sizeof(*data));
    ZVAL_COPY(&data->arch_zv, arch_zv);
    arch_object *obj = arch_object_from_zv(arch_zv);
    data->generation = obj->entry_generation;
    data->remaining_bytes = obj->current_entry_size; /* -1 if not set */
    php_stream *stream =
            php_stream_alloc(&php_stream_arch_ops, data, NULL, "rb");
    return stream;
}
