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

/* ── Compile-time DJBX33A hashes (matches zend_inline_hash_func) ──────── */
#define _DJB_STEP(h, c) (((zend_ulong)(h) << 5) + (zend_ulong)(h) + (zend_ulong)(unsigned char)(c))
#define _DJB_0         ((zend_ulong)5381)
#define _DJB_1(a)                              _DJB_STEP(_DJB_0, a)
#define _DJB_2(a,b)                            _DJB_STEP(_DJB_1(a), b)
#define _DJB_3(a,b,c)                          _DJB_STEP(_DJB_2(a,b), c)
#define _DJB_4(a,b,c,d)                        _DJB_STEP(_DJB_3(a,b,c), d)
#define _DJB_5(a,b,c,d,e)                      _DJB_STEP(_DJB_4(a,b,c,d), e)
#define _DJB_6(a,b,c,d,e,f)                    _DJB_STEP(_DJB_5(a,b,c,d,e), f)
#define _DJB_7(a,b,c,d,e,f,g)                  _DJB_STEP(_DJB_6(a,b,c,d,e,f), g)
#define _DJB_8(a,b,c,d,e,f,g,h)               _DJB_STEP(_DJB_7(a,b,c,d,e,f,g), h)
#define _DJB_9(a,b,c,d,e,f,g,h,i)             _DJB_STEP(_DJB_8(a,b,c,d,e,f,g,h), i)
#define _DJB_10(a,b,c,d,e,f,g,h,i,j)          _DJB_STEP(_DJB_9(a,b,c,d,e,f,g,h,i), j)
#define _DJB_11(a,b,c,d,e,f,g,h,i,j,k)        _DJB_STEP(_DJB_10(a,b,c,d,e,f,g,h,i,j), k)
#define _DJB_COUNT_(a,b,c,d,e,f,g,h,i,j,k,N,...) N
#define _DJB_COUNT(...) _DJB_COUNT_(__VA_ARGS__,11,10,9,8,7,6,5,4,3,2,1)
#define _DJB_PASTE(a,b) a##b
#define _DJB_SEL(n)     _DJB_PASTE(_DJB_,n)
/* zend_inline_hash_func sets the high bit of the result */
#define DJB(...) (_DJB_SEL(_DJB_COUNT(__VA_ARGS__))(__VA_ARGS__) | ((zend_ulong)1 << (8 * sizeof(zend_ulong) - 1)))

#define ENTRY_HASH_PATHNAME      DJB('p','a','t','h','n','a','m','e')
#define ENTRY_HASH_SIZE          DJB('s','i','z','e')
#define ENTRY_HASH_PERM          DJB('p','e','r','m')
#define ENTRY_HASH_MTIME         DJB('m','t','i','m','e')
#define ENTRY_HASH_CTIME         DJB('c','t','i','m','e')
#define ENTRY_HASH_TYPE          DJB('t','y','p','e')
#define ENTRY_HASH_IS_FILE       DJB('i','s','F','i','l','e')
#define ENTRY_HASH_IS_DIR        DJB('i','s','D','i','r')
#define ENTRY_HASH_IS_SYMLINK    DJB('i','s','S','y','m','l','i','n','k')
#define ENTRY_HASH_SYMLINK       DJB('s','y','m','l','i','n','k')
#define ENTRY_HASH_HARDLINK      DJB('h','a','r','d','l','i','n','k')
#define ENTRY_HASH_UID           DJB('u','i','d')
#define ENTRY_HASH_GID           DJB('g','i','d')
#define ENTRY_HASH_UNAME         DJB('u','n','a','m','e')
#define ENTRY_HASH_GNAME         DJB('g','n','a','m','e')
#define ENTRY_HASH_ATIME         DJB('a','t','i','m','e')
#define ENTRY_HASH_RDEV          DJB('r','d','e','v')
#define ENTRY_HASH_IS_ENCRYPTED  DJB('i','s','E','n','c','r','y','p','t','e','d')

/* ── Per-property accessors ──────────────────────────────────────────────── */
static inline void entry_prop_pathname(struct archive_entry *entry, zval *rv)
{
    const char *s = archive_entry_pathname_utf8(entry);
    if (!s) ZVAL_NULL(rv); else ZVAL_STRING(rv, s);
}
static inline void entry_prop_size(struct archive_entry *entry, zval *rv)
{
    if (!archive_entry_size_is_set(entry)) ZVAL_NULL(rv);
    else ZVAL_LONG(rv, archive_entry_size(entry));
}
static inline void entry_prop_perm(struct archive_entry *entry, zval *rv)
{
    ZVAL_LONG(rv, archive_entry_perm(entry));
}
static inline void entry_prop_mtime(struct archive_entry *entry, zval *rv)
{
    if (!archive_entry_mtime_is_set(entry)) ZVAL_NULL(rv);
    else ZVAL_LONG(rv, archive_entry_mtime(entry));
}
static inline void entry_prop_ctime(struct archive_entry *entry, zval *rv)
{
    if (!archive_entry_ctime_is_set(entry)) ZVAL_NULL(rv);
    else ZVAL_LONG(rv, archive_entry_ctime(entry));
}
static inline void entry_prop_atime(struct archive_entry *entry, zval *rv)
{
    if (!archive_entry_atime_is_set(entry)) ZVAL_NULL(rv);
    else ZVAL_LONG(rv, archive_entry_atime(entry));
}
static inline void entry_prop_type(struct archive_entry *entry, zval *rv)
{
    ZVAL_LONG(rv, archive_entry_filetype(entry));
}
static inline void entry_prop_is_file(struct archive_entry *entry, zval *rv)
{
    ZVAL_BOOL(rv, archive_entry_filetype(entry) == AE_IFREG);
}
static inline void entry_prop_is_dir(struct archive_entry *entry, zval *rv)
{
    ZVAL_BOOL(rv, archive_entry_filetype(entry) == AE_IFDIR);
}
static inline void entry_prop_is_symlink(struct archive_entry *entry, zval *rv)
{
    ZVAL_BOOL(rv, archive_entry_filetype(entry) == AE_IFLNK);
}
static inline void entry_prop_symlink(struct archive_entry *entry, zval *rv)
{
    const char *s = archive_entry_symlink_utf8(entry);
    if (!s) ZVAL_NULL(rv); else ZVAL_STRING(rv, s);
}
static inline void entry_prop_hardlink(struct archive_entry *entry, zval *rv)
{
    const char *s = archive_entry_hardlink_utf8(entry);
    if (!s) ZVAL_NULL(rv); else ZVAL_STRING(rv, s);
}
static inline void entry_prop_uid(struct archive_entry *entry, zval *rv)
{
    ZVAL_LONG(rv, archive_entry_uid(entry));
}
static inline void entry_prop_gid(struct archive_entry *entry, zval *rv)
{
    ZVAL_LONG(rv, archive_entry_gid(entry));
}
static inline void entry_prop_uname(struct archive_entry *entry, zval *rv)
{
    const char *s = archive_entry_uname_utf8(entry);
    if (!s) ZVAL_NULL(rv); else ZVAL_STRING(rv, s);
}
static inline void entry_prop_gname(struct archive_entry *entry, zval *rv)
{
    const char *s = archive_entry_gname_utf8(entry);
    if (!s) ZVAL_NULL(rv); else ZVAL_STRING(rv, s);
}
static inline void entry_prop_rdev(struct archive_entry *entry, zval *rv)
{
    ZVAL_LONG(rv, archive_entry_rdev(entry));
}
static inline void entry_prop_is_encrypted(struct archive_entry *entry, zval *rv)
{
#if ARCHIVE_VERSION_NUMBER >= 3002000
    ZVAL_BOOL(rv, archive_entry_is_data_encrypted(entry)
                  || archive_entry_is_metadata_encrypted(entry));
#else
    ZVAL_FALSE(rv);
#endif
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

    switch (zend_string_hash_val(member)) {
    case ENTRY_HASH_PATHNAME:
        if (!zend_string_equals_literal(member, "pathname")) break;
        entry_prop_pathname(entry, rv); return rv;
    case ENTRY_HASH_SIZE:
        if (!zend_string_equals_literal(member, "size")) break;
        entry_prop_size(entry, rv); return rv;
    case ENTRY_HASH_PERM:
        if (!zend_string_equals_literal(member, "perm")) break;
        entry_prop_perm(entry, rv); return rv;
    case ENTRY_HASH_MTIME:
        if (!zend_string_equals_literal(member, "mtime")) break;
        entry_prop_mtime(entry, rv); return rv;
    case ENTRY_HASH_CTIME:
        if (!zend_string_equals_literal(member, "ctime")) break;
        entry_prop_ctime(entry, rv); return rv;
    case ENTRY_HASH_TYPE:
        if (!zend_string_equals_literal(member, "type")) break;
        entry_prop_type(entry, rv); return rv;
    case ENTRY_HASH_IS_FILE:
        if (!zend_string_equals_literal(member, "isFile")) break;
        entry_prop_is_file(entry, rv); return rv;
    case ENTRY_HASH_IS_DIR:
        if (!zend_string_equals_literal(member, "isDir")) break;
        entry_prop_is_dir(entry, rv); return rv;
    case ENTRY_HASH_IS_SYMLINK:
        if (!zend_string_equals_literal(member, "isSymlink")) break;
        entry_prop_is_symlink(entry, rv); return rv;
    case ENTRY_HASH_SYMLINK:
        if (!zend_string_equals_literal(member, "symlink")) break;
        entry_prop_symlink(entry, rv); return rv;
    case ENTRY_HASH_HARDLINK:
        if (!zend_string_equals_literal(member, "hardlink")) break;
        entry_prop_hardlink(entry, rv); return rv;
    case ENTRY_HASH_UID:
        if (!zend_string_equals_literal(member, "uid")) break;
        entry_prop_uid(entry, rv); return rv;
    case ENTRY_HASH_GID:
        if (!zend_string_equals_literal(member, "gid")) break;
        entry_prop_gid(entry, rv); return rv;
    case ENTRY_HASH_UNAME:
        if (!zend_string_equals_literal(member, "uname")) break;
        entry_prop_uname(entry, rv); return rv;
    case ENTRY_HASH_GNAME:
        if (!zend_string_equals_literal(member, "gname")) break;
        entry_prop_gname(entry, rv); return rv;
    case ENTRY_HASH_ATIME:
        if (!zend_string_equals_literal(member, "atime")) break;
        entry_prop_atime(entry, rv); return rv;
    case ENTRY_HASH_RDEV:
        if (!zend_string_equals_literal(member, "rdev")) break;
        entry_prop_rdev(entry, rv); return rv;
    case ENTRY_HASH_IS_ENCRYPTED:
        if (!zend_string_equals_literal(member, "isEncrypted")) break;
        entry_prop_is_encrypted(entry, rv); return rv;
    }

    return &EG(uninitialized_zval);
}
static zend_array *entry_oh_get_properties_for(zend_object *object, zend_prop_purpose purpose)
{
    entry_object *entry_obj = entry_object_fetch(object);
    struct archive_entry *entry = entry_obj->entry;

    zend_array *props = zend_new_array(18);

    if (entry == NULL) {
        return props;
    }

    zval tmp;
#define ADD(key, fn) \
    fn(entry, &tmp); \
    zend_hash_str_add_new(props, key, sizeof(key) - 1, &tmp);

    ADD("pathname",    entry_prop_pathname)
    ADD("size",        entry_prop_size)
    ADD("perm",        entry_prop_perm)
    ADD("mtime",       entry_prop_mtime)
    ADD("ctime",       entry_prop_ctime)
    ADD("atime",       entry_prop_atime)
    ADD("type",        entry_prop_type)
    ADD("isFile",      entry_prop_is_file)
    ADD("isDir",       entry_prop_is_dir)
    ADD("isSymlink",   entry_prop_is_symlink)
    ADD("symlink",     entry_prop_symlink)
    ADD("hardlink",    entry_prop_hardlink)
    ADD("uid",         entry_prop_uid)
    ADD("gid",         entry_prop_gid)
    ADD("uname",       entry_prop_uname)
    ADD("gname",       entry_prop_gname)
    ADD("rdev",        entry_prop_rdev)
    ADD("isEncrypted", entry_prop_is_encrypted)

#undef ADD

    return props;
}
static int entry_oh_has_property(zend_object *object, zend_string *member,
                                 int has_set_exists, void **cache_slot)
{
    entry_object *entry_obj = entry_object_fetch(object);
    struct archive_entry *entry = entry_obj->entry;

    if (entry == NULL) {
        return 0;
    }

    bool known;
    switch (zend_string_hash_val(member)) {
    case ENTRY_HASH_PATHNAME:     known = zend_string_equals_literal(member, "pathname");    break;
    case ENTRY_HASH_SIZE:         known = zend_string_equals_literal(member, "size");        break;
    case ENTRY_HASH_PERM:         known = zend_string_equals_literal(member, "perm");        break;
    case ENTRY_HASH_MTIME:        known = zend_string_equals_literal(member, "mtime");       break;
    case ENTRY_HASH_CTIME:        known = zend_string_equals_literal(member, "ctime");       break;
    case ENTRY_HASH_TYPE:         known = zend_string_equals_literal(member, "type");        break;
    case ENTRY_HASH_IS_FILE:      known = zend_string_equals_literal(member, "isFile");      break;
    case ENTRY_HASH_IS_DIR:       known = zend_string_equals_literal(member, "isDir");       break;
    case ENTRY_HASH_IS_SYMLINK:   known = zend_string_equals_literal(member, "isSymlink");   break;
    case ENTRY_HASH_SYMLINK:      known = zend_string_equals_literal(member, "symlink");     break;
    case ENTRY_HASH_HARDLINK:     known = zend_string_equals_literal(member, "hardlink");    break;
    case ENTRY_HASH_UID:          known = zend_string_equals_literal(member, "uid");         break;
    case ENTRY_HASH_GID:          known = zend_string_equals_literal(member, "gid");         break;
    case ENTRY_HASH_UNAME:        known = zend_string_equals_literal(member, "uname");       break;
    case ENTRY_HASH_GNAME:        known = zend_string_equals_literal(member, "gname");       break;
    case ENTRY_HASH_ATIME:        known = zend_string_equals_literal(member, "atime");       break;
    case ENTRY_HASH_RDEV:         known = zend_string_equals_literal(member, "rdev");        break;
    case ENTRY_HASH_IS_ENCRYPTED: known = zend_string_equals_literal(member, "isEncrypted"); break;
    default:                      known = false; break;
    }

    if (!known) {
        return 0;
    }

    if (has_set_exists == ZEND_PROPERTY_EXISTS) {
        return 1;
    }

    zval tmp;
    zval *val = entry_oh_read_property(object, member, BP_VAR_IS, cache_slot, &tmp);
    int result = (has_set_exists == ZEND_PROPERTY_ISSET)
        ? Z_TYPE_P(val) != IS_NULL
        : zend_is_true(val);
    zval_ptr_dtor(&tmp);
    return result;
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
    if (zend_string_hash_val(member) == ENTRY_HASH_PATHNAME
            && zend_string_equals_literal(member, "pathname")) {
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

    zobj->source_kind = ARCH_SOURCE_NONE;
    zobj->write_disk_options = 0;
    zobj->formats = NULL;
    zobj->formats_count = 0;
    zobj->filters = NULL;
    zobj->filters_count = 0;
    zobj->archive = NULL;
    zobj->arch_disk = NULL;
    zobj->write_disk_options = 0;
    zobj->entry_generation = 0;
    zobj->current_entry_size = -1;
    memset(&zobj->password_fci, 0, sizeof(zobj->password_fci));
    memset(&zobj->password_fcc, 0, sizeof(zobj->password_fcc));
    zobj->password_cb_last_result = NULL;
    zend_object_std_init(&zobj->parent, ce);
    zobj->parent.handlers = &arch_oh;

    return &zobj->parent;
}
static void arch_oh_free_obj(zend_object *zobj)
{
    arch_object *obj = arch_object_fetch(zobj);
    switch (obj->source_kind) {
        case ARCH_SOURCE_FILE:
            zend_string_release(obj->source.file_location);
            break;
        case ARCH_SOURCE_STREAM:
            zval_ptr_dtor(&obj->source.stream_zv);
            break;
        case ARCH_SOURCE_NONE:
            break;
    }
    obj->source_kind = ARCH_SOURCE_NONE;
    if (obj->formats) {
        efree(obj->formats);
        obj->formats = NULL;
        obj->formats_count = 0;
    }
    if (obj->filters) {
        efree(obj->filters);
        obj->filters = NULL;
        obj->filters_count = 0;
    }
    if (obj->archive) {
        archive_read_free(obj->archive);
        obj->archive = NULL;
    }
    if (obj->arch_disk) {
        (void)archive_write_close(obj->arch_disk);
        (void)archive_write_free(obj->arch_disk);
        obj->arch_disk = NULL;
    }
    if (obj->password_fci.size != 0) {
        zval_ptr_dtor(&obj->password_fci.function_name);
        obj->password_fci.size = 0;
    }
    if (obj->password_cb_last_result) {
        zend_string_release(obj->password_cb_last_result);
        obj->password_cb_last_result = NULL;
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
    arch_obj->source_kind = ARCH_SOURCE_FILE;
    arch_obj->source.file_location = zend_string_copy(file);
    arch_obj->write_disk_options = (int)flags;
}

static bool arch_obj_open_read_stream(arch_object *arch_obj);

static const char *arch_passphrase_callback(struct archive *a, void *_client_data)
{
    (void)a;
    arch_object *arch_obj = (arch_object *)_client_data;

    zval retval;
    ZVAL_UNDEF(&retval);
    arch_obj->password_fci.retval = &retval;
    arch_obj->password_fci.param_count = 0;
    arch_obj->password_fci.params = NULL;

    if (zend_call_function(&arch_obj->password_fci, &arch_obj->password_fcc) == FAILURE
            || Z_TYPE(retval) == IS_UNDEF) {
        return NULL;
    }

    if (arch_obj->password_cb_last_result) {
        zend_string_release(arch_obj->password_cb_last_result);
        arch_obj->password_cb_last_result = NULL;
    }

    if (Z_TYPE(retval) == IS_NULL) {
        return NULL;
    }

    arch_obj->password_cb_last_result = zval_get_string(&retval);
    zval_ptr_dtor(&retval);
    return ZSTR_VAL(arch_obj->password_cb_last_result);
}

static void arch_obj_setup_support(arch_object *arch_obj)
{
    if (arch_obj->filters == NULL) {
        archive_read_support_filter_all(arch_obj->archive);
    } else {
#if ARCHIVE_VERSION_NUMBER >= 3005000
        for (uint32_t i = 0; i < arch_obj->filters_count; i++) {
            archive_read_support_filter_by_code(arch_obj->archive, arch_obj->filters[i]);
        }
#else
        archive_read_support_filter_all(arch_obj->archive);
#endif
    }
    if (arch_obj->formats == NULL) {
        archive_read_support_format_all(arch_obj->archive);
    } else {
        for (uint32_t i = 0; i < arch_obj->formats_count; i++) {
            archive_read_support_format_by_code(arch_obj->archive, arch_obj->formats[i]);
        }
    }
}

static bool arch_obj_open_read(arch_object *arch_obj)
{
    php_stream *stream = php_stream_open_wrapper(
            arch_obj->source.file_location->val, "rb",
            REPORT_ERRORS | STREAM_WILL_CAST | PHP_STREAM_PREFER_STDIO |
                    STREAM_MUST_SEEK,
            NULL);

    if (!stream) {
        zend_throw_exception_ex(except_ce, -1, "Could not open %s",
                                arch_obj->source.file_location->val);
        return false;
    }

    int fd;
    if (php_stream_can_cast(stream, PHP_STREAM_AS_FD) == SUCCESS &&
            php_stream_cast(stream, PHP_STREAM_AS_FD, (void **)&fd, 0) == SUCCESS) {
        arch_obj->archive = archive_read_new();
        arch_obj_setup_support(arch_obj);
        if (arch_obj->password_fci.size != 0) {
            archive_read_set_passphrase_callback(arch_obj->archive, arch_obj,
                                                 arch_passphrase_callback);
        }
        int res = archive_read_open_fd(arch_obj->archive, fd, 10240);
        if (res != ARCHIVE_OK) {
            zend_throw_exception_ex(
                    except_ce, archive_errno(arch_obj->archive),
                    "Could not open archive from file descriptor: %s",
                    archive_error_string(arch_obj->archive));
            return false;
        }
    } else {
        /* FD cast not supported by this stream wrapper; fall back to FILE*.
         * Switch the source so arch_obj_open_read_stream can take over and
         * the stream resource is kept alive for the lifetime of the archive. */
        zend_string_release(arch_obj->source.file_location);
        arch_obj->source_kind = ARCH_SOURCE_STREAM;
        php_stream_to_zval(stream, &arch_obj->source.stream_zv);
        return arch_obj_open_read_stream(arch_obj);
    }

    return true;
}

static bool arch_obj_open_read_stream(arch_object *arch_obj)
{
    php_stream *stream;
    php_stream_from_zval_no_verify(stream, &arch_obj->source.stream_zv);
    if (!stream) {
        zend_throw_exception(except_ce, "Invalid stream resource", -1);
        return false;
    }

    FILE *fp;
    int res = php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void **)&fp, REPORT_ERRORS);
    if (res != SUCCESS) {
        zend_throw_exception(except_ce, "Could not cast stream to FILE*", -1);
        return false;
    }

    arch_obj->archive = archive_read_new();
    arch_obj_setup_support(arch_obj);
    if (arch_obj->password_fci.size != 0) {
        archive_read_set_passphrase_callback(arch_obj->archive, arch_obj,
                                             arch_passphrase_callback);
    }
    res = archive_read_open_FILE(arch_obj->archive, fp);
    if (res != ARCHIVE_OK) {
        zend_throw_exception_ex(
                except_ce, archive_errno(arch_obj->archive),
                "Could not open archive from stream: %s",
                archive_error_string(arch_obj->archive));
        return false;
    }

    return true;
}

PHP_METHOD(libarchive_Archive, fromStream)
{
    zval *stream_zv;
    zend_long flags = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &stream_zv, &flags) ==
        FAILURE) {
        return;
    }

    object_init_ex(return_value, arch_ce);
    arch_object *arch_obj = arch_object_from_zv(return_value);
    arch_obj->source_kind = ARCH_SOURCE_STREAM;
    ZVAL_COPY(&arch_obj->source.stream_zv, stream_zv);
    arch_obj->write_disk_options = (int)flags;
}

PHP_METHOD(libarchive_Archive, withPasswordCallback)
{
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_FUNC(fci, fcc)
    ZEND_PARSE_PARAMETERS_END();

    arch_object *arch_obj = arch_object_from_zv(getThis());
    if (arch_obj->archive != NULL) {
        zend_throw_exception(except_ce,
                "Cannot set password callback after archive has been opened", -1);
        return;
    }

    Z_TRY_ADDREF(fci.function_name);
    if (arch_obj->password_fci.size != 0) {
        zval_ptr_dtor(&arch_obj->password_fci.function_name);
    }
    arch_obj->password_fci = fci;
    arch_obj->password_fcc = fcc;

    RETURN_THIS();
}

PHP_METHOD(libarchive_Archive, supportFormats)
{
    zval *args;
    uint32_t num_args;
    ZEND_PARSE_PARAMETERS_START(1, -1)
        Z_PARAM_VARIADIC('+', args, num_args)
    ZEND_PARSE_PARAMETERS_END();

    arch_object *arch_obj = arch_object_from_zv(getThis());
    if (arch_obj->archive != NULL) {
        zend_throw_exception(except_ce,
                "Cannot change format after archive has been opened", -1);
        return;
    }
    efree(arch_obj->formats);
    arch_obj->formats = safe_emalloc(num_args, sizeof(int), 0);
    arch_obj->formats_count = num_args;
    for (uint32_t i = 0; i < num_args; i++) {
        arch_obj->formats[i] = (int)Z_LVAL(args[i]);
    }
    RETURN_THIS();
}

PHP_METHOD(libarchive_Archive, supportFilters)
{
    zval *args;
    uint32_t num_args;
    ZEND_PARSE_PARAMETERS_START(1, -1)
        Z_PARAM_VARIADIC('+', args, num_args)
    ZEND_PARSE_PARAMETERS_END();

    arch_object *arch_obj = arch_object_from_zv(getThis());
    if (arch_obj->archive != NULL) {
        zend_throw_exception(except_ce,
                "Cannot change filter after archive has been opened", -1);
        return;
    }
#if ARCHIVE_VERSION_NUMBER < 3005000
    php_error_docref(NULL, E_WARNING,
            "supportFilters() requires libarchive >= 3.5.0; all filters will be enabled");
    RETURN_THIS();
#endif
    efree(arch_obj->filters);
    arch_obj->filters = safe_emalloc(num_args, sizeof(int), 0);
    arch_obj->filters_count = num_args;
    for (uint32_t i = 0; i < num_args; i++) {
        arch_obj->filters[i] = (int)Z_LVAL(args[i]);
    }
    RETURN_THIS();
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
    if (arch_obj->source_kind == ARCH_SOURCE_NONE) {
        php_error_docref(
                NULL, E_ERROR,
                "The Archive object has not been properly constructed");
    }

    if (arch_obj->archive != NULL) {
        zend_throw_exception(except_ce, "The archive has been opened before",
                             -1);
        return NULL;
    }
    if (arch_obj->source_kind == ARCH_SOURCE_FILE) {
        if (!arch_obj_open_read(arch_obj)) {
            return NULL;
        }
    } else {
        if (!arch_obj_open_read_stream(arch_obj)) {
            return NULL;
        }
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
    entry_oh.has_property = entry_oh_has_property;
    entry_oh.get_property_ptr_ptr = entry_oh_get_property_ptr_ptr;
    entry_oh.write_property = entry_oh_write_property;
    entry_oh.get_properties_for = entry_oh_get_properties_for;
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
