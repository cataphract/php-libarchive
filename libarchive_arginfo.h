/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 81dea0d19df4df4a10af9d7c03bd825e47795f43 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_libarchive_Entry___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_libarchive_Archive___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_libarchive_Archive_fromStream, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_libarchive_Archive_supportFormats, 0, 1, IS_STATIC, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, formats, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_libarchive_Archive_supportFilters, 0, 1, IS_STATIC, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, filters, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_libarchive_Archive_extractCurrent, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, entry, libarchive\\Entry, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_libarchive_Archive_currentEntryStream, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_libarchive_Archive_getIterator, 0, 0, Traversable, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(libarchive_Entry, __construct);
ZEND_METHOD(libarchive_Archive, __construct);
ZEND_METHOD(libarchive_Archive, fromStream);
ZEND_METHOD(libarchive_Archive, supportFormats);
ZEND_METHOD(libarchive_Archive, supportFilters);
ZEND_METHOD(libarchive_Archive, extractCurrent);
ZEND_METHOD(libarchive_Archive, currentEntryStream);
ZEND_METHOD(libarchive_Archive, getIterator);


static const zend_function_entry class_libarchive_Exception_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_libarchive_Entry_methods[] = {
	ZEND_ME(libarchive_Entry, __construct, arginfo_class_libarchive_Entry___construct, ZEND_ACC_PRIVATE)
	ZEND_FE_END
};


static const zend_function_entry class_libarchive_Archive_methods[] = {
	ZEND_ME(libarchive_Archive, __construct, arginfo_class_libarchive_Archive___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(libarchive_Archive, fromStream, arginfo_class_libarchive_Archive_fromStream, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(libarchive_Archive, supportFormats, arginfo_class_libarchive_Archive_supportFormats, ZEND_ACC_PUBLIC)
	ZEND_ME(libarchive_Archive, supportFilters, arginfo_class_libarchive_Archive_supportFilters, ZEND_ACC_PUBLIC)
	ZEND_ME(libarchive_Archive, extractCurrent, arginfo_class_libarchive_Archive_extractCurrent, ZEND_ACC_PUBLIC)
	ZEND_ME(libarchive_Archive, currentEntryStream, arginfo_class_libarchive_Archive_currentEntryStream, ZEND_ACC_PUBLIC)
	ZEND_ME(libarchive_Archive, getIterator, arginfo_class_libarchive_Archive_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static void register_libarchive_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_NONE", ARCHIVE_FILTER_NONE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_GZIP", ARCHIVE_FILTER_GZIP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_BZIP2", ARCHIVE_FILTER_BZIP2, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_COMPRESS", ARCHIVE_FILTER_COMPRESS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_LZMA", ARCHIVE_FILTER_LZMA, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_XZ", ARCHIVE_FILTER_XZ, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_UU", ARCHIVE_FILTER_UU, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_RPM", ARCHIVE_FILTER_RPM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_LZIP", ARCHIVE_FILTER_LZIP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_LRZIP", ARCHIVE_FILTER_LRZIP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_LZOP", ARCHIVE_FILTER_LZOP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_GRZIP", ARCHIVE_FILTER_GRZIP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_LZ4", ARCHIVE_FILTER_LZ4, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FILTER_ZSTD", ARCHIVE_FILTER_ZSTD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_CPIO", ARCHIVE_FORMAT_CPIO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_SHAR", ARCHIVE_FORMAT_SHAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_TAR", ARCHIVE_FORMAT_TAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_ISO9660", ARCHIVE_FORMAT_ISO9660, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_ZIP", ARCHIVE_FORMAT_ZIP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_EMPTY", ARCHIVE_FORMAT_EMPTY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_AR", ARCHIVE_FORMAT_AR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_MTREE", ARCHIVE_FORMAT_MTREE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_RAW", ARCHIVE_FORMAT_RAW, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_XAR", ARCHIVE_FORMAT_XAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_LHA", ARCHIVE_FORMAT_LHA, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_CAB", ARCHIVE_FORMAT_CAB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_RAR", ARCHIVE_FORMAT_RAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_7ZIP", ARCHIVE_FORMAT_7ZIP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_WARC", ARCHIVE_FORMAT_WARC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\FORMAT_RAR_V5", ARCHIVE_FORMAT_RAR_V5, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_OWNER", ARCHIVE_EXTRACT_OWNER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_PERM", ARCHIVE_EXTRACT_PERM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_TIME", ARCHIVE_EXTRACT_TIME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_NO_OVERWRITE", ARCHIVE_EXTRACT_NO_OVERWRITE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_UNLINK", ARCHIVE_EXTRACT_UNLINK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_ACL", ARCHIVE_EXTRACT_ACL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_FFLAGS", ARCHIVE_EXTRACT_FFLAGS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_XATTR", ARCHIVE_EXTRACT_XATTR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_SECURE_SYMLINKS", ARCHIVE_EXTRACT_SECURE_SYMLINKS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_SECURE_NODOTDOT", ARCHIVE_EXTRACT_SECURE_NODOTDOT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_NO_AUTODIR", ARCHIVE_EXTRACT_NO_AUTODIR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_NO_OVERWRITE_NEWER", ARCHIVE_EXTRACT_NO_OVERWRITE_NEWER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_SPARSE", ARCHIVE_EXTRACT_SPARSE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_MAC_METADATA", ARCHIVE_EXTRACT_MAC_METADATA, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_NO_HFS_COMPRESSION", ARCHIVE_EXTRACT_NO_HFS_COMPRESSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_HFS_COMPRESSION_FORCED", ARCHIVE_EXTRACT_HFS_COMPRESSION_FORCED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_SECURE_NOABSOLUTEPATHS", ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("libarchive\\EXTRACT_CLEAR_NOCHANGE_FFLAGS", ARCHIVE_EXTRACT_CLEAR_NOCHANGE_FFLAGS, CONST_PERSISTENT);
}

static zend_class_entry *register_class_libarchive_Exception(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "libarchive", "Exception", class_libarchive_Exception_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_libarchive_Entry(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "libarchive", "Entry", class_libarchive_Entry_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_libarchive_Archive(zend_class_entry *class_entry_IteratorAggregate)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "libarchive", "Archive", class_libarchive_Archive_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 1, class_entry_IteratorAggregate);

	return class_entry;
}
