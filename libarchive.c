/* libarchive extension for PHP */

#include "zend_API.h"
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <php.h>
#include <zend_interfaces.h>
#include <zend_exceptions.h>
#include <ext/standard/info.h>
#include <ext/date/php_date.h>
#include <archive.h>
#include <archive_entry.h>
#include "php_libarchive.h"
#include "stream.h"
#include <stdbool.h>

#if PHP_MAJOR_VERSION >= 8
typedef zend_object handler_this_t;
typedef zend_string handler_member_t;
#else
typedef zval handler_this_t;
typedef zval handler_member_t;
#endif

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
static inline entry_object *entry_object_from_handler_this(handler_this_t *obj)
{
#if PHP_MAJOR_VERSION >= 8
    return entry_object_fetch(obj);
#else
    return entry_object_from_zv(obj);
#endif
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
    zval_dtor(&obj->archive_obj);
    ZVAL_UNDEF(&obj->archive_obj);
    zend_object_std_dtor(zobj);
}
#if PHP_MAJOR_VERSION < 8
static inline void zend_string_release_p(zend_string **p) {
    zend_string_release(*p);
}
#endif
static zval *entry_oh_read_property(handler_this_t *object,
                                    handler_member_t *member, int type,
                                    void **cache_slot, zval *rv)
{
    entry_object *entry_obj = entry_object_from_handler_this(object);
#if PHP_MAJOR_VERSION >= 8
    zend_string *member_str = member;
#else
    zend_string *__attribute__((__cleanup__(zend_string_release_p))) member_str =
            zval_get_string(member);
#endif

    struct archive_entry *entry = entry_obj->entry;
    if (entry == NULL) {
        ZVAL_NULL(rv);
        return rv;
    }

    if (zend_string_equals_literal(member_str, "pathname")) {
        const char *pathname = archive_entry_pathname_utf8(entry);
        if (!pathname) {
            ZVAL_NULL(rv);
        } else {
            ZVAL_STRING(rv, pathname);
        }
        return rv;
    } else if (zend_string_equals_literal(member_str, "size")) {
        if (!archive_entry_size_is_set(entry)) {
            ZVAL_NULL(rv);
        } else {
            ZVAL_LONG(rv, archive_entry_size(entry));
        }
        return rv;
    } else if (zend_string_equals_literal(member_str, "perm")) {
        ZVAL_LONG(rv, archive_entry_perm(entry));
        return rv;
    } else if (zend_string_equals_literal(member_str, "mtime")) {
        if (!archive_entry_mtime_is_set(entry)) {
            ZVAL_NULL(rv);
        } else {
            ZVAL_LONG(rv, archive_entry_mtime(entry));
        }
        return rv;
    } else if (zend_string_equals_literal(member_str, "ctime")) {
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
static zval *entry_oh_get_property_ptr_ptr(handler_this_t *object,
                                           handler_member_t *member, int type,
                                           void **cache_slot)
{
    return NULL; // force engine to fallback on read+write when possible
}
#if PHP_VERSION_ID < 70400
typedef void write_prop_ret_t;
#else
typedef zval *write_prop_ret_t;
#endif
static write_prop_ret_t entry_oh_write_property(handler_this_t *object,
                                                handler_member_t *member,
                                                zval *value,
                                                void **cache_slot)
{
    entry_object *entry_obj = entry_object_from_handler_this(object);
#if PHP_MAJOR_VERSION >= 8
    zend_string *member_str = member;
#else
    zend_string *__attribute__((__cleanup__(zend_string_release_p)))
        member_str = zval_get_string(member);
#endif

    struct archive_entry *entry = entry_obj->entry;
    if (entry == NULL) {
        zend_throw_exception(except_ce, "Entry not initialized", -1);
        goto end;
    }
    if (zend_string_equals_literal(member_str, "pathname")) {
        zend_string *val = zval_get_string(value);
        archive_entry_set_pathname_utf8(entry, val->val);
        zend_string_release(val);
    } else {
        zend_throw_exception_ex(
                except_ce, -1,"Not a known writable property: %s",
                member_str->val);
    }

end:
#if PHP_VERSION_ID >= 70400
    return value;
#else
	;
#endif
}

ZEND_BEGIN_ARG_INFO(arginfo_entry___construct, 0)
ZEND_END_ARG_INFO();
static PHP_METHOD(Entry, __construct)
{
    zend_throw_exception(except_ce, "This method should not be called", -1);
}
static const zend_function_entry entry_functions[] = {
    PHP_ME(Entry, __construct, arginfo_entry___construct, ZEND_ACC_PRIVATE)
    PHP_FE_END
};
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_arch___construct, 2, false, 1)
ZEND_ARG_INFO(0, "file")
ZEND_ARG_INFO(0, "flags")
ZEND_END_ARG_INFO();
static PHP_METHOD(Archive, __construct)
{
    zend_string *file;
    zend_long flags = 0;
    if (zend_parse_parameters_throw(
            ZEND_NUM_ARGS(), "S|l", &file, &flags) == FAILURE) {
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
        REPORT_ERRORS | STREAM_WILL_CAST | PHP_STREAM_PREFER_STDIO | STREAM_MUST_SEEK,
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
        zend_throw_exception_ex(except_ce, archive_errno(arch_obj->archive),
                                "Could not open archive from file descriptor: %s",
                                archive_error_string(arch_obj->archive));
        return false;
    }

    return true;
}

ZEND_BEGIN_ARG_INFO(arginfo_arch_cur_entry_stream, 0)
ZEND_END_ARG_INFO();
static PHP_METHOD(Archive, currentEntryStream)
{
    php_stream *stream = php_stream_arch_cur_entry_stream(getThis());
    php_stream_to_zval(stream, return_value);
}

ZEND_BEGIN_ARG_INFO(arginfo_arch_extract, 1)
ZEND_ARG_OBJ_INFO(0, entry, libarchive\\Entry, 0)
ZEND_END_ARG_INFO();
static bool copy_data(struct archive *ar, struct archive *arch_disk);
static PHP_METHOD(Archive, extractCurrent)
{
    zval *entry_zv;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS(), "O", &entry_zv, entry_ce) == FAILURE) {
        return;
    }

    entry_object *entry_obj = entry_object_from_zv(entry_zv);
    if (entry_obj->entry == NULL) { // should not happen
        return;
    }
    const char *path = archive_entry_pathname_utf8(entry_obj->entry);
    if (!path) {
        zend_throw_exception(except_ce,
                "Entry has no path or it's not convertible to UTF-8", -1);
        return;
    }
    if (path[0] != '/') {
        char cwd[MAXPATHLEN];
        char *result = VCWD_GETCWD(cwd, sizeof(cwd));
        if (!result) {
            zend_throw_exception(
                    except_ce, "Could not obtain current directory", -1);
            return;
        }
        size_t would_len = strlcat(cwd, path, sizeof(cwd));
        if (would_len >= sizeof(cwd)) {
            zend_throw_exception(
                    except_ce, "After prepending current directory, "
                               "the pathname is too long", -1);
            return;
        }
        archive_entry_set_pathname_utf8(entry_obj->entry, cwd);
        path =  archive_entry_pathname_utf8(entry_obj->entry);
        if (!path) {
            zend_throw_exception(except_ce,
                                 "After prepending current directory, "
                                 "could not set the entry pathname", -1);
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
        zend_throw_exception_ex(
                except_ce, archive_errno(arch_obj->arch_disk),
                "Could not write file metadata: %s",
                archive_error_string(arch_obj->arch_disk));
        return;
    }

    if (!copy_data(arch_obj->archive, arch_obj->arch_disk)) {
        return;
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
            zend_throw_exception_ex(
                    except_ce, archive_errno(ar),
                    "Error reading data block: %s", archive_error_string(ar));
            return false;
        }
        r = archive_write_data_block(aw, buff, size, offset);
        if (r != ARCHIVE_OK) {
            zend_throw_exception_ex(
                    except_ce, archive_errno(ar),
                    "Error writing data block: %s", archive_error_string(aw));
            return false;
        }
    }
}

#if PHP_MAJOR_VERSION >= 8
ZEND_BEGIN_ARG_INFO(arginfo_arch_get_iterator, 0)
ZEND_END_ARG_INFO();
static PHP_METHOD(Archive, getIterator)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}
#endif

static const zend_function_entry arch_functions[] = {
    PHP_ME(Archive, __construct, arginfo_arch___construct, ZEND_ACC_PUBLIC)
    PHP_ME(Archive, extractCurrent, arginfo_arch_extract, ZEND_ACC_PUBLIC)
    PHP_ME(Archive, currentEntryStream, arginfo_arch_cur_entry_stream, ZEND_ACC_PUBLIC)
#if PHP_MAJOR_VERSION >= 8
    PHP_ME(Archive, getIterator, arginfo_arch_get_iterator, ZEND_ACC_PUBLIC)
#endif
    PHP_FE_END
};

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
static zend_object_iterator *arch_ce_get_iterator(
    zend_class_entry *ce, zval *object, int by_ref)
{
    if (by_ref) {
        php_error_docref(NULL, E_ERROR,
                         "An iterator cannot be used with foreach by reference");
    }

    arch_object *arch_obj = arch_object_from_zv(object);
    if (arch_obj->file_location == NULL) {
        php_error_docref(NULL, E_ERROR,
                         "The Archive object has not been properly constructed");
    }

    if (arch_obj->archive != NULL) {
        zend_throw_exception(except_ce, "The archive has been opened before", -1);
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

#if PHP_VERSION_ID < 70300
	it->parent.funcs = ce->iterator_funcs.funcs;
#else
	it->parent.funcs = &arch_it_funcs;
#endif

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

    arch_it_invalidate_current((zend_object_iterator *) it);
    zval_ptr_dtor(&it->parent.data); // reduce refcount on archive
    it->finished = true;
}
static int arch_it_valid(zend_object_iterator *iter)
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
    arch_it_dtor,
    arch_it_valid,
    arch_it_current_data,
    NULL,
    arch_it_move_forward,
    NULL,
    arch_it_invalidate_current
};

/* end Archive }}} */

static PHP_MINIT_FUNCTION(libarchive)
{
    // initialize libarchive\Exception
    {
        zend_class_entry except_temp_ce;
        zend_function_entry entries[] = {PHP_FE_END};
        INIT_CLASS_ENTRY(except_temp_ce, "libarchive\\Exception", entries);
        except_ce = zend_register_internal_class_ex(
            &except_temp_ce, zend_exception_get_default());
    }
    except_ce->ce_flags |= ZEND_ACC_FINAL;

    // initialize libarchive\Archive
    arch_oh = *zend_get_std_object_handlers();
    arch_oh.clone_obj = NULL;
    arch_oh.free_obj = arch_oh_free_obj;
    arch_oh.offset = XtOffsetOf(arch_object, parent);
    {
        zend_class_entry arch_temp_ce;
        INIT_CLASS_ENTRY(arch_temp_ce, "libarchive\\Archive", arch_functions)
        arch_ce = zend_register_internal_class(&arch_temp_ce);
    }
    arch_ce->ce_flags |= ZEND_ACC_FINAL;
    arch_ce->clone = NULL;
    arch_ce->create_object = arch_ce_create_object;
    arch_ce->get_iterator = arch_ce_get_iterator;
#if PHP_VERSION_ID < 70300
    arch_ce->iterator_funcs.funcs = &arch_it_funcs;
#endif
#if PHP_MAJOR_VERSION >= 8
    zend_class_implements(arch_ce, 1, zend_ce_aggregate);
#else
    zend_class_implements(arch_ce, 1, zend_ce_traversable);
#endif

    // initialize libarchive\Entry
    entry_oh = *zend_get_std_object_handlers();
    entry_oh.read_property = entry_oh_read_property;
    entry_oh.get_property_ptr_ptr = entry_oh_get_property_ptr_ptr;
    entry_oh.write_property = entry_oh_write_property;
    // TODO: override get_properties and has_property
    entry_oh.clone_obj = NULL;
    entry_oh.free_obj = entry_oh_free_obj;
    entry_oh.offset = XtOffsetOf(entry_object, parent);
    {
        zend_class_entry entry_temp_ce;
        INIT_CLASS_ENTRY(entry_temp_ce, "libarchive\\Entry", entry_functions);
        entry_ce = zend_register_internal_class(&entry_temp_ce);
    }
    entry_ce->ce_flags |= ZEND_ACC_FINAL;
    entry_ce->clone = NULL;
    entry_ce->constructor = NULL; /* disallow instantiation with new */
    entry_ce->create_object = entry_ce_create_object;

#define REGISTER_EXTRACT_CONST(name) \
    REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_" #name, \
                           ARCHIVE_EXTRACT_ ## name, CONST_CS | CONST_PERSISTENT);

   REGISTER_EXTRACT_CONST(OWNER);
   REGISTER_EXTRACT_CONST(PERM);
   REGISTER_EXTRACT_CONST(TIME);
   REGISTER_EXTRACT_CONST(NO_OVERWRITE);
   REGISTER_EXTRACT_CONST(UNLINK);
   REGISTER_EXTRACT_CONST(ACL);
   REGISTER_EXTRACT_CONST(FFLAGS);
   REGISTER_EXTRACT_CONST(XATTR);
   REGISTER_EXTRACT_CONST(SECURE_SYMLINKS);
   REGISTER_EXTRACT_CONST(SECURE_NODOTDOT);
   REGISTER_EXTRACT_CONST(NO_AUTODIR);
   REGISTER_EXTRACT_CONST(NO_OVERWRITE_NEWER);
   REGISTER_EXTRACT_CONST(SPARSE);
   REGISTER_EXTRACT_CONST(MAC_METADATA);
   REGISTER_EXTRACT_CONST(NO_HFS_COMPRESSION);
   REGISTER_EXTRACT_CONST(HFS_COMPRESSION_FORCED);
   REGISTER_EXTRACT_CONST(SECURE_NOABSOLUTEPATHS);
   REGISTER_EXTRACT_CONST(CLEAR_NOCHANGE_FFLAGS);

   return SUCCESS;
}

/* {{{ libarchive_module_entry
 */
static zend_module_entry libarchive_module_entry = {
    STANDARD_MODULE_HEADER,
    "libarchive",                    /* Extension name */
    NULL,                         /* zend_function_entry */
    PHP_MINIT(libarchive),        /* PHP_MINIT - Module initialization */
    NULL,                         /* PHP_MSHUTDOWN - Module shutdown */
    NULL,                         /* PHP_RINIT - Request initialization */
    NULL,                         /* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(libarchive),        /* PHP_MINFO - Module info */
    PHP_LIBARCHIVE_VERSION,       /* Version */
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ARCHIVE
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(libarchive)
#endif
