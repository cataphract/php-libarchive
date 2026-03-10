#pragma once

#if PHP_VERSION_ID < 80400
static inline zend_class_entry *php_libarchive_register_class_with_flags(
        zend_class_entry *ce, zend_class_entry *parent_ce, uint32_t flags)
{
    zend_class_entry *registered =
            zend_register_internal_class_ex(ce, parent_ce);
    registered->ce_flags |= flags;
    return registered;
}
#define zend_register_internal_class_with_flags \
    php_libarchive_register_class_with_flags
#endif
