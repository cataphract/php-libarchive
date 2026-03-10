/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d41c11ce5a6a024796710fff87e0402e77446549 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_libarchive_Entry___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_libarchive_Archive___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
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
ZEND_METHOD(libarchive_Archive, extractCurrent);
ZEND_METHOD(libarchive_Archive, currentEntryStream);
ZEND_METHOD(libarchive_Archive, getIterator);

static const zend_function_entry class_libarchive_Entry_methods[] = {
	ZEND_ME(libarchive_Entry, __construct, arginfo_class_libarchive_Entry___construct, ZEND_ACC_PRIVATE)
	ZEND_FE_END
};

static const zend_function_entry class_libarchive_Archive_methods[] = {
	ZEND_ME(libarchive_Archive, __construct, arginfo_class_libarchive_Archive___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(libarchive_Archive, extractCurrent, arginfo_class_libarchive_Archive_extractCurrent, ZEND_ACC_PUBLIC)
	ZEND_ME(libarchive_Archive, currentEntryStream, arginfo_class_libarchive_Archive_currentEntryStream, ZEND_ACC_PUBLIC)
	ZEND_ME(libarchive_Archive, getIterator, arginfo_class_libarchive_Archive_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static void register_libarchive_symbols(int module_number)
{
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

	INIT_NS_CLASS_ENTRY(ce, "libarchive", "Exception", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_libarchive_Entry(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "libarchive", "Entry", class_libarchive_Entry_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_libarchive_Archive(zend_class_entry *class_entry_IteratorAggregate)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "libarchive", "Archive", class_libarchive_Archive_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
	zend_class_implements(class_entry, 1, class_entry_IteratorAggregate);

	return class_entry;
}
