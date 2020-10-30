#ifndef STREAM_H
#define STREAM_H

#include "php_libarchive.h"

php_stream *php_stream_arch_cur_entry_stream(zval *nonnull arch_zv);

#endif
