# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Common Changelog](https://common-changelog.org/).

## [Unreleased]

### Added

- **Windows build support.** Added `config.w32` with automatic detection of
  optional static compression libraries (zlib, bzip2, lzma, lz4, zstd, OpenSSL)
  from a given install prefix.
- **CI: Linux test matrix.** GitHub Actions workflow (`tests.yml`) that runs the
  test suite across multiple PHP versions and variants (debug/release, ZTS/NTS)
  using Docker images, with JUnit artifact upload.
- **CI: Windows tests.** Windows CI job in `tests.yml` that builds and tests the
  extension on Windows using `php/php-windows-builder` and a statically-linked
  libarchive from vcpkg.
- **CI: Release workflow.** `release.yml` builds Windows binaries for PHP
  8.0–8.5 (x64) via vcpkg and uploads them as a GitHub draft release on version
  tags.
- **`libarchive.stub.php`.** Canonical PHP API definition file with full
  docblock documentation for all classes, methods, and constants. Used to
  generate `libarchive_arginfo.h` via `gen_stub.php`.
- **`libarchive_arginfo.h`.** Generated header with typed argument info, method
  tables, `register_class_*` helpers, and a `register_libarchive_symbols()`
  function replacing hand-written `ZEND_BEGIN_ARG_INFO` macros and constant
  registration.
- **`php_compat.h`.** Compatibility shim that back-ports
  `zend_register_internal_class_with_flags()` for PHP versions before 8.4.
- **`composer.json`.** PIE (PHP Installer for Extensions) manifest, declaring
  the package as `cataphract/libarchive` with the `php-ext` type and a
  `--with-libarchive` configure option.
- **`README.md`.** Usage examples for extracting to disk and reading an entry as
  a PHP stream, plus API and installation notes.
- **`Justfile`.** Developer task runner with recipes for building, testing, and
  updating Docker image SHAs.
- **`.github/docker-image-shas.yml` and `update-docker-shas.sh`.** Pinned Docker
  image SHAs for the CI matrix and a script to refresh them.
- **Test archive corpus.** Added fixture archives covering all major formats
  (tar/ustar/GNU/pax, zip, 7-zip, cpio/odc/bin, cab, iso, rpm, xar, WARC, ar)
  and compression filters (gzip, bzip2, xz, lzma, lz4, zstd, lzo, lrzip,
  compress, uuencode, lzip).
- **`tests/stream-all-formats.phpt`.** Test that reads the content of one entry
  from each fixture archive via `currentEntryStream()`, verifying all
  format/filter combinations work end to end.
- **`tests/stream-invalidation.phpt`.** Test that verifies a stream obtained
  from `currentEntryStream()` emits `E_WARNING` and returns no data after the
  iterator has advanced to the next entry.
- **`entry_generation` and `current_entry_size` fields in `arch_object`.**
  Internal counters used to detect stale streams and to cap reads at the
  declared entry size.
- Extension name is now "archive", matching the solib name "archive.so".

### Fixed

- **Stream invalidation after iterator advance.** A PHP stream returned by
  `currentEntryStream()` is now considered stale as soon as the archive iterator
  moves to the next entry. Reading from a stale stream emits `E_WARNING` and
  returns `false`, preventing silent corruption where data from a subsequent
  entry would be returned.
- **Stream reads capped at declared entry size.** `php_arch_ops_read()` now
  tracks `remaining_bytes` (initialised from `archive_entry_size`) and caps
  reads accordingly, ensuring `stream_get_contents()` terminates correctly
  instead of blocking or returning extra bytes.
- **File handle leak in `extractCurrent()` on failure.**
  `archive_write_finish_entry()` is now always called even when `copy_data()`
  fails. Previously the open file handle was kept alive until the `Archive`
  object was freed, blocking `unlink()` on Windows.
- **Absolute path detection on Windows in `extractCurrent()`.** Replaced the
  Unix-only `path[0] != '/'` check with the PHP macro `IS_ABSOLUTE_PATH(path,
  len)`, which also recognises Windows drive-letter and UNC paths.
- **Pathname compatibility with libarchive < 3.6.2.**
  `archive_entry_update_pathname_utf8()` is now used instead of
  `archive_entry_set_pathname_utf8()` when writing the `pathname` property.
  Older libarchive versions (before commit d6248d2) did not fallback to UTF-8
  when fetching the pathname causing an error due to the absence of any path.
- **`zval_dtor` → `zval_ptr_dtor` in `entry_oh_free_obj`.** The previous call
  decremented the refcount without running destructors; the correct function is
  `zval_ptr_dtor`.

### Changed

- **Minimum PHP version is now 8.0.** All PHP 7.x compatibility guards have been
  removed: `handler_this_t`, `handler_member_t`, `write_prop_ret_t`,
  `stream_ret_t` as `size_t`, `zend_string_release_p`, the pre-7.3 iterator
  funcs setup, and the pre-8.0 `getIterator`/`zend_ce_traversable` code.
- **Extension internal name changed to `archive`.** The `zend_module_entry`
  `.name` field and `config.w32 EXTENSION()` call now use `"archive"` instead of
  `"libarchive"`, aligning the extension name with the `COMPILE_DL_ARCHIVE`
  symbol and PHP convention.
- **Class registration uses generated stub code.** `PHP_MINIT_FUNCTION` now
  calls `register_class_libarchive_Exception()`,
  `register_class_libarchive_Archive()`, and `register_class_libarchive_Entry()`
  from `libarchive_arginfo.h`, replacing ad-hoc `INIT_CLASS_ENTRY` /
  `zend_register_internal_class` blocks and `REGISTER_EXTRACT_CONST` macros.

[Unreleased]: https://github.com/cataphract/php-libarchive/compare/e27e0e973f721804b4f343b81a6f8a2462984144...HEAD

<!-- vim: set tw=80: -->
