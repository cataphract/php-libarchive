/* libarchive extension for PHP */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <zend_interfaces.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>
#include <archive.h>
#include <archive_entry.h>
#include "php_libarchive.h"
#include "php_compat.h"
#include "stream.h"
#include "libarchive_arginfo.h"
#include <stdbool.h>

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(libarchive)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "libarchive support", "enabled");
    php_info_print_table_row(2, "archive extension version", PHP_LIBARCHIVE_VERSION);
    php_info_print_table_row(2, "libarchive version", ARCHIVE_VERSION_ONLY_STRING);
    php_info_print_table_row(2, "libarchive details", archive_version_details());
    php_info_print_table_end();
}
/* }}} */

zend_class_entry *except_ce;

/* {{{ Entry */
static zend_class_entry *entry_ce;
typedef struct _entry_object {
    struct archive_entry *nullable entry;
    zval archive_obj;
    zend_object parent;
} entry_object;

static zend_object_handlers entry_oh;
static inline entry_object *entry_object_fetch(zend_object *zobj)
{
    return (entry_object *)((char *)zobj - XtOffsetOf(entry_object, parent));
}
static inline entry_object *entry_object_from_zv(const zval *zv)
{
    return entry_object_fetch(Z_OBJ_P(zv));
}

static zend_object *entry_ce_create_object(zend_class_entry *ce)
{
    entry_object *zobj =
            emalloc(sizeof(*zobj) + zend_object_properties_size(ce));

    zobj->entry = NULL;
    ZVAL_UNDEF(&zobj->archive_obj);
    zend_object_std_init(&zobj->parent, ce);
    zobj->parent.handlers = &entry_oh;

    return &zobj->parent;
}
static void entry_oh_free_obj(zend_object *zobj)
{
    entry_object *obj = entry_object_fetch(zobj);
    if (obj->entry) {
        archive_entry_free(obj->entry);
        obj->entry = NULL;
    }
    zval_ptr_dtor(&obj->archive_obj);
    ZVAL_UNDEF(&obj->archive_obj);
    zend_object_std_dtor(zobj);
}

static zval *entry_oh_read_property(zend_object *object, zend_string *member,
                                    int type, void **cache_slot, zval *rv)
{
    entry_object *entry_obj = entry_object_fetch(object);

    struct archive_entry *entry = entry_obj->entry;
    if (entry == NULL) {
        ZVAL_NULL(rv);
        return rv;
    }

    if (zend_string_equals_literal(member, "pathname")) {
        const char *pathname = archive_entry_pathname_utf8(entry);
        if (!pathname) {
            ZVAL_NULL(rv);
        } else {
            ZVAL_STRING(rv, pathname);
        }
        return rv;
    } else if (zend_string_equals_literal(member, "size")) {
        if (!archive_entry_size_is_set(entry)) {
            ZVAL_NULL(rv);
        } else {
            ZVAL_LONG(rv, archive_entry_size(entry));
        }
        return rv;
    } else if (zend_string_equals_literal(member, "perm")) {
        ZVAL_LONG(rv, archive_entry_perm(entry));
        return rv;
    } else if (zend_string_equals_literal(member, "mtime")) {
        if (!archive_entry_mtime_is_set(entry)) {
            ZVAL_NULL(rv);
        } else {
            ZVAL_LONG(rv, archive_entry_mtime(entry));
        }
        return rv;
    } else if (zend_string_equals_literal(member, "ctime")) {
        if (!archive_entry_ctime_is_set(entry)) {
            ZVAL_NULL(rv);
        } else {
            ZVAL_LONG(rv, archive_entry_ctime(entry));
        }
        return rv;
    } else {
        return &EG(uninitialized_zval);
    }
}
static zval *entry_oh_get_property_ptr_ptr(zend_object *object,
                                           zend_string *member, int type,
                                           void **cache_slot)
{
    return NULL; // force engine to fallback on read+write when possible
}
static zval *entry_oh_write_property(zend_object *object, zend_string *member,
                                     zval *value, void **cache_slot)
{
    entry_object *entry_obj = entry_object_fetch(object);

    struct archive_entry *entry = entry_obj->entry;
    if (entry == NULL) {
        zend_throw_exception(except_ce, "Entry not initialized", -1);
        return value;
    }
    if (zend_string_equals_literal(member, "pathname")) {
        zend_string *val = zval_get_string(value);
        /* Versions of libarchive before
         * https://github.com/libarchive/libarchive/commit/d6248d2640efe3e40a1a9c0bb7bd8903f6beef98
         * (3.6.2) do not use a fallback to archive_entry_pathname() in case
         * MBS is not available. So call archive_entry_update_pathname_utf8()
         * instead of archive_entry_set_pathname_utf8(). The former will try
         * to populate the MBS name (and the WCS name) as well by converting
         * the UTF-8 name. This will prevent errors about invalid empty
         * pathnames when extracting in older versions. */
        archive_entry_update_pathname_utf8(entry, val->val);
        zend_string_release(val);
    } else {
        zend_throw_exception_ex(except_ce, -1,
                                "Not a known writable property: %s",
                                member->val);
    }

    return value;
}

PHP_METHOD(libarchive_Entry, __construct)
{
    zend_throw_exception(except_ce, "This method should not be called", -1);
}
/* end Entry }}} */

/* {{{ Archive */
static zend_class_entry *arch_ce;
static zend_object_handlers arch_oh;
static zend_object *arch_ce_create_object(zend_class_entry *ce)
{
    arch_object *zobj =
            emalloc(sizeof(*zobj) + zend_object_properties_size(ce));

    zobj->file_location = NULL;
    zobj->archive = NULL;
    zobj->arch_disk = NULL;
    zobj->write_disk_options = 0;
    zobj->entry_generation = 0;
    zobj->current_entry_size = -1;
    zend_object_std_init(&zobj->parent, ce);
    zobj->parent.handlers = &arch_oh;

    return &zobj->parent;
}
static void arch_oh_free_obj(zend_object *zobj)
{
    arch_object *obj = arch_object_fetch(zobj);
    if (obj->file_location) {
        zend_string_release(obj->file_location);
        obj->file_location = NULL;
    }
    if (obj->archive) {
        archive_read_close(obj->archive);
        obj->archive = NULL;
    }
    if (obj->arch_disk) {
        (void)archive_write_close(obj->arch_disk);
        (void)archive_write_free(obj->arch_disk);
        obj->arch_disk = NULL;
    }
    zend_object_std_dtor(zobj);
}

PHP_METHOD(libarchive_Archive, __construct)
{
    zend_string *file;
    zend_long flags = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|l", &file, &flags) ==
        FAILURE) {
        return;
    }

    arch_object *arch_obj = arch_object_from_zv(getThis());
    arch_obj->file_location = zend_string_copy(file);
    arch_obj->write_disk_options = (int)flags;
}

static bool arch_obj_open_read(arch_object *arch_obj)
{
    php_stream *stream = php_stream_open_wrapper(
            arch_obj->file_location->val, "rb",
            REPORT_ERRORS | STREAM_WILL_CAST | PHP_STREAM_PREFER_STDIO |
                    STREAM_MUST_SEEK,
            NULL);

    if (!stream) {
        zend_throw_exception_ex(except_ce, -1, "Could not open %s",
                                arch_obj->file_location->val);
        return false;
    }

    int fd;
    int res = php_stream_cast(stream, PHP_STREAM_AS_FD, (void **)&fd, 0);
    if (res != SUCCESS) {
        zend_throw_exception_ex(except_ce, -1, "Could not cast stream for %s",
                                arch_obj->file_location->val);
        return false;
    }

    arch_obj->archive = archive_read_new();
    archive_read_support_filter_all(arch_obj->archive);
    archive_read_support_format_all(arch_obj->archive);
    res = archive_read_open_fd(arch_obj->archive, fd, 10240);
    if (res != ARCHIVE_OK) {
        zend_throw_exception_ex(
                except_ce, archive_errno(arch_obj->archive),
                "Could not open archive from file descriptor: %s",
                archive_error_string(arch_obj->archive));
        return false;
    }

    return true;
}

PHP_METHOD(libarchive_Archive, currentEntryStream)
{
    ZEND_PARSE_PARAMETERS_NONE();

    php_stream *stream = php_stream_arch_cur_entry_stream(getThis());
    php_stream_to_zval(stream, return_value);
}

static bool copy_data(struct archive *ar, struct archive *arch_disk);
PHP_METHOD(libarchive_Archive, extractCurrent)
{
    zval *entry_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &entry_zv, entry_ce) ==
        FAILURE) {
        return;
    }

    entry_object *entry_obj = entry_object_from_zv(entry_zv);
    if (entry_obj->entry == NULL) { // should not happen
        return;
    }
    const char *path = archive_entry_pathname(entry_obj->entry);
    if (!path) {
        zend_throw_exception(
                except_ce, "Entry has no path or it's not convertible to UTF-8",
                -1);
        return;
    }
    if (!IS_ABSOLUTE_PATH(path, strlen(path))) {
        char cwd[MAXPATHLEN];
        char *result = VCWD_GETCWD(cwd, sizeof(cwd));
        if (!result) {
            zend_throw_exception(except_ce,
                                 "Could not obtain current directory", -1);
            return;
        }
        size_t would_len = strlcat(cwd, path, sizeof(cwd));
        if (would_len >= sizeof(cwd)) {
            zend_throw_exception(except_ce,
                                 "After prepending current directory, "
                                 "the pathname is too long",
                                 -1);
            return;
        }
        archive_entry_set_pathname(entry_obj->entry, cwd);
        path = archive_entry_pathname(entry_obj->entry);
        if (!path) {
            zend_throw_exception(except_ce,
                                 "After prepending current directory, "
                                 "could not set the entry pathname",
                                 -1);
            return;
        }
    }
    if (php_check_open_basedir(path)) {
        zend_throw_exception_ex(except_ce, -1,
                                "open_basedir check failed for %s", path);
        return;
    }

    arch_object *arch_obj = arch_object_from_zv(getThis());
    if (!arch_obj->archive) {
        zend_throw_exception(except_ce, "The archive is not open", -1);
        return;
    }
    if (!arch_obj->arch_disk) { // ensure arch_disk
        arch_obj->arch_disk = archive_write_disk_new();
        archive_write_disk_set_options(arch_obj->arch_disk,
                                       arch_obj->write_disk_options);
        archive_write_disk_set_standard_lookup(arch_obj->arch_disk);
    }

    int res = archive_write_header(arch_obj->arch_disk, entry_obj->entry);
    if (res != ARCHIVE_OK) {
        zend_throw_exception_ex(except_ce, archive_errno(arch_obj->arch_disk),
                                "Could not write file metadata: %s",
                                archive_error_string(arch_obj->arch_disk));
        return;
    }

    bool data_ok = copy_data(arch_obj->archive, arch_obj->arch_disk);

    /* Always finish the entry to close the file handle, even if copy_data
     * failed — otherwise the handle stays open until the object is freed,
     * blocking unlink() on Windows. */
    res = archive_write_finish_entry(arch_obj->arch_disk);
    if (!data_ok) {
        return;
    }
    if (res != ARCHIVE_OK) {
        zend_throw_exception_ex(except_ce, archive_errno(arch_obj->arch_disk),
                                "Could not finish writing entry: %s",
                                archive_error_string(arch_obj->arch_disk));
    }
}

static bool copy_data(struct archive *ar, struct archive *aw)
{
    int r;
    const void *buff;
    size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
    int64_t offset;
#else
    off_t offset;
#endif

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF) {
            return true;
        }
        if (r != ARCHIVE_OK) {
            zend_throw_exception_ex(except_ce, archive_errno(ar),
                                    "Error reading data block: %s",
                                    archive_error_string(ar));
            return false;
        }
        r = archive_write_data_block(aw, buff, size, offset);
        if (r != ARCHIVE_OK) {
            zend_throw_exception_ex(except_ce, archive_errno(ar),
                                    "Error writing data block: %s",
                                    archive_error_string(aw));
            return false;
        }
    }
}

PHP_METHOD(libarchive_Archive, getIterator)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

// Archive iterator
typedef struct _arch_iterator {
    zend_object_iterator parent;
    zval value;
    // IteratorIterator invalidates current and then checks for validity
    // we can't rely on a move_forward between invalidation and current/valid
    bool finished;
} arch_iterator;

static void arch_it_populate_with_next(arch_iterator *it);
static zend_object_iterator_funcs arch_it_funcs;
static zend_object_iterator *arch_ce_get_iterator(zend_class_entry *ce,
                                                  zval *object, int by_ref)
{
    if (by_ref) {
        php_error_docref(
                NULL, E_ERROR,
                "An iterator cannot be used with foreach by reference");
    }

    arch_object *arch_obj = arch_object_from_zv(object);
    if (arch_obj->file_location == NULL) {
        php_error_docref(
                NULL, E_ERROR,
                "The Archive object has not been properly constructed");
    }

    if (arch_obj->archive != NULL) {
        zend_throw_exception(except_ce, "The archive has been opened before",
                             -1);
        return NULL;
    }
    if (!arch_obj_open_read(arch_obj)) {
        return NULL;
    }

    arch_iterator *it = emalloc(sizeof *it);

    zend_iterator_init((zend_object_iterator *)it);
    ZVAL_COPY(&it->parent.data, object);
    ZVAL_UNDEF(&it->value);
    it->finished = false;
    it->parent.funcs = &arch_it_funcs;

    return (zend_object_iterator *)it;
}

// populate it->value
static void arch_it_populate_with_next(arch_iterator *it)
{
    assert(Z_TYPE(it->value) == IS_UNDEF);

    arch_object *arch_obj = arch_object_from_zv(&it->parent.data);
    if (!arch_obj->archive) {
        return;
    }

    // could be improved by recycling the entries
    struct archive_entry *entry = archive_entry_new2(arch_obj->archive);
    arch_obj->entry_generation++;
    int res = archive_read_next_header2(arch_obj->archive, entry);
    if (res == ARCHIVE_EOF) {
        it->finished = true;
        archive_entry_free(entry);
        return;
    } else if (res != ARCHIVE_OK) {
        it->finished = true;
        archive_entry_free(entry);
        zend_throw_exception_ex(except_ce, archive_errno(arch_obj->archive),
                                "Error moving to next header: %s",
                                archive_error_string(arch_obj->archive));
        return;
    }

    res = object_init_ex(&it->value, entry_ce);
    if (res == FAILURE) {
        it->finished = true;
        archive_entry_free(entry);
        return;
    }
    entry_object *entry_obj = entry_object_from_zv(&it->value);
    entry_obj->entry = entry;
    ZVAL_DUP(&entry_obj->archive_obj, &it->parent.data);
    arch_obj->current_entry_size =
            archive_entry_size_is_set(entry) ? archive_entry_size(entry) : -1;
}

static void arch_it_invalidate_current(zend_object_iterator *iter)
{
    arch_iterator *it = (arch_iterator *)iter;
    if (Z_TYPE_INFO(it->value) != IS_UNDEF) {
        zval_ptr_dtor(&it->value);
        ZVAL_UNDEF(&it->value);
    }
}
static void arch_it_dtor(zend_object_iterator *iter)
{
    arch_iterator *it = (arch_iterator *)iter;

    arch_it_invalidate_current((zend_object_iterator *)it);
    zval_ptr_dtor(&it->parent.data); // reduce refcount on archive
    it->finished = true;
}
static zend_result arch_it_valid(zend_object_iterator *iter)
{
    arch_iterator *it = (arch_iterator *)iter;
    if (Z_ISUNDEF(it->value) && !it->finished) {
        arch_it_populate_with_next(it);
    }
    return ((arch_iterator *)iter)->finished ? FAILURE : SUCCESS;
}
static zval *arch_it_current_data(zend_object_iterator *iter)
{
    arch_iterator *it = (arch_iterator *)iter;

    if (Z_ISUNDEF(it->value) && !it->finished) {
        arch_it_populate_with_next(it);
    }
    return &it->value;
}
static void arch_it_move_forward(zend_object_iterator *iter)
{
    arch_iterator *it = (arch_iterator *)iter;
    arch_it_invalidate_current((zend_object_iterator *)it);
}

static zend_object_iterator_funcs arch_it_funcs = {
        arch_it_dtor,         arch_it_valid, arch_it_current_data,      NULL,
        arch_it_move_forward, NULL,          arch_it_invalidate_current};

/* end Archive }}} */

static PHP_MINIT_FUNCTION(libarchive)
{
    // initialize libarchive\Exception
    except_ce = register_class_libarchive_Exception(zend_ce_exception);

    // initialize libarchive\Archive
    arch_oh = *zend_get_std_object_handlers();
    arch_oh.clone_obj = NULL;
    arch_oh.free_obj = arch_oh_free_obj;
    arch_oh.offset = XtOffsetOf(arch_object, parent);
    arch_ce = register_class_libarchive_Archive(zend_ce_aggregate);
    arch_ce->create_object = arch_ce_create_object;
    arch_ce->get_iterator = arch_ce_get_iterator;

    // initialize libarchive\Entry
    entry_oh = *zend_get_std_object_handlers();
    entry_oh.read_property = entry_oh_read_property;
    entry_oh.get_property_ptr_ptr = entry_oh_get_property_ptr_ptr;
    entry_oh.write_property = entry_oh_write_property;
    entry_oh.clone_obj = NULL;
    entry_oh.free_obj = entry_oh_free_obj;
    entry_oh.offset = XtOffsetOf(entry_object, parent);
    entry_ce = register_class_libarchive_Entry();
    entry_ce->create_object = entry_ce_create_object;

    // initialize constants
    register_libarchive_symbols(module_number);

    return SUCCESS;
}

/* {{{ libarchive_module_entry
 */
static zend_module_entry libarchive_module_entry = {
        STANDARD_MODULE_HEADER,
        .name = "archive",
        .functions = NULL,
        .module_startup_func = PHP_MINIT(libarchive),
        .module_shutdown_func = NULL,
        .request_startup_func = NULL,
        .request_shutdown_func = NULL,
        .info_func = PHP_MINFO(libarchive),
        .version = PHP_LIBARCHIVE_VERSION,
        STANDARD_MODULE_PROPERTIES};
/* }}} */

#ifdef COMPILE_DL_ARCHIVE
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(libarchive)
#endif
