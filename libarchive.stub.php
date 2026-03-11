<?php

/** @generate-class-entries */

namespace libarchive {

    /**
     * Restore the user and group ownership of extracted entries.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_OWNER.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_OWNER
     */
    const EXTRACT_OWNER = UNKNOWN;

    /**
     * Restore the file-permission bits (mode) of extracted entries.
     * Without this flag the umask is applied to every extracted file.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_PERM.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_PERM
     */
    const EXTRACT_PERM = UNKNOWN;

    /**
     * Restore the modification and access timestamps of extracted entries.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_TIME.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_TIME
     */
    const EXTRACT_TIME = UNKNOWN;

    /**
     * Do not overwrite existing files on disk. By default files are
     * overwritten.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_NO_OVERWRITE.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_NO_OVERWRITE
     */
    const EXTRACT_NO_OVERWRITE = UNKNOWN;

    /**
     * Unlink (remove) each file from the filesystem before recreating it.
     * Useful when the archive contains hard-linked files that must replace
     * a different file type already on disk.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_UNLINK.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_UNLINK
     */
    const EXTRACT_UNLINK = UNKNOWN;

    /**
     * Restore POSIX.1e Access Control Lists if the filesystem supports them.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_ACL.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_ACL
     */
    const EXTRACT_ACL = UNKNOWN;

    /**
     * Restore BSD file flags (chflags) such as nodump and sappend, where
     * supported by the operating system and filesystem.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_FFLAGS.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_FFLAGS
     */
    const EXTRACT_FFLAGS = UNKNOWN;

    /**
     * Restore extended attributes (xattrs) if the filesystem supports them.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_XATTR.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_XATTR
     */
    const EXTRACT_XATTR = UNKNOWN;

    /**
     * Refuse to extract entries that would write through a symbolic link
     * that already exists on disk. Prevents certain directory-traversal
     * attacks.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_SECURE_SYMLINKS.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_SECURE_SYMLINKS
     */
    const EXTRACT_SECURE_SYMLINKS = UNKNOWN;

    /**
     * Refuse to extract entries whose pathnames contain ".." components.
     * Prevents directory-traversal attacks that escape the target directory.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_SECURE_NODOTDOT.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_SECURE_NODOTDOT
     */
    const EXTRACT_SECURE_NODOTDOT = UNKNOWN;

    /**
     * Do not automatically create intermediate directories that are not
     * explicitly listed as entries in the archive.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_NO_AUTODIR.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_NO_AUTODIR
     */
    const EXTRACT_NO_AUTODIR = UNKNOWN;

    /**
     * Do not overwrite an existing file that is newer than the entry being
     * extracted.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_NO_OVERWRITE_NEWER.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_NO_OVERWRITE_NEWER
     */
    const EXTRACT_NO_OVERWRITE_NEWER = UNKNOWN;

    /**
     * Write extracted files as sparse files where the filesystem supports
     * it. Long runs of zero bytes in the extracted data are converted to
     * holes rather than written explicitly.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_SPARSE.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_SPARSE
     */
    const EXTRACT_SPARSE = UNKNOWN;

    /**
     * Restore Mac OS X–specific extended metadata stored in AppleDouble
     * sidecar files or via the copyfile(3) mechanism. Has no effect on
     * non-Apple platforms.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_MAC_METADATA.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_MAC_METADATA
     */
    const EXTRACT_MAC_METADATA = UNKNOWN;

    /**
     * Do not apply HFS+ compression to extracted files, even if the
     * compressed attribute was set in the archive metadata. macOS only.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_NO_HFS_COMPRESSION.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_NO_HFS_COMPRESSION
     */
    const EXTRACT_NO_HFS_COMPRESSION = UNKNOWN;

    /**
     * Apply HFS+ compression to every extracted regular file, regardless
     * of whether the archive metadata requested it. macOS only.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_HFS_COMPRESSION_FORCED.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_HFS_COMPRESSION_FORCED
     */
    const EXTRACT_HFS_COMPRESSION_FORCED = UNKNOWN;

    /**
     * Refuse to extract entries whose pathnames are absolute (begin with
     * "/"). Prevents archives from overwriting files outside the intended
     * extraction directory.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS
     */
    const EXTRACT_SECURE_NOABSOLUTEPATHS = UNKNOWN;

    /**
     * Clear immutable and append-only BSD file flags on existing files
     * before attempting to overwrite them. Without this flag, extraction
     * may fail when such flags are set.
     * Equivalent to the libarchive flag ARCHIVE_EXTRACT_CLEAR_NOCHANGE_FFLAGS.
     *
     * @var int
     * @cvalue ARCHIVE_EXTRACT_CLEAR_NOCHANGE_FFLAGS
     */
    const EXTRACT_CLEAR_NOCHANGE_FFLAGS = UNKNOWN;

    /**
     * Exception thrown by the libarchive extension on any error.
     *
     * The exception code is the errno value returned by libarchive for
     * errors that originate from the C library; it is -1 for errors
     * originating within the PHP extension itself.
     */
    final class Exception extends \Exception {}

    /**
     * Represents a single entry inside an archive (file, directory,
     * symbolic link, etc.).
     *
     * Entry objects are produced exclusively by iterating over an
     * {@see Archive}; they cannot be constructed directly.
     *
     * The entry metadata is stored inside the underlying libarchive
     * archive_entry struct and is exposed as virtual properties via the
     * object-handler layer. Only {@see Entry::$pathname} is writable;
     * all other properties are read-only.
     *
     * @property string|null $pathname
     *   The entry path as a UTF-8 string, or null if the path is not
     *   available or cannot be represented in UTF-8. Writable: assigning
     *   to this property updates the underlying archive_entry struct,
     *   which affects subsequent calls to {@see Archive::extractCurrent()}.
     *
     * @property int|null $size
     *   The uncompressed size of the entry in bytes, or null if the size
     *   is not recorded in the archive header.
     *
     * @property int $perm
     *   The lower 12 permission bits of the file mode (equivalent to
     *   {@link https://man.archlinux.org/man/stat.2 stat(2)}'s st_mode &
     *   07777). Always available; entries that carry no explicit
     *   permission information return 0.
     *
     * @property int|null $mtime
     *   The last-modified timestamp as a Unix epoch integer, or null if
     *   not set in the archive header.
     *
     * @property int|null $ctime
     *   The last-status-change timestamp as a Unix epoch integer, or null
     *   if not set in the archive header.
     */
    final class Entry
    {
        /** @internal Entries are created by the Archive iterator. */
        private function __construct() {}
    }

    /**
     * Read-only archive reader supporting all formats and filters
     * recognised by the system libarchive (tar, zip, 7-zip, cpio, …,
     * gzip, bzip2, xz, zstd, …).
     *
     * The archive file is opened lazily: the constructor only stores the
     * path and the extraction flags. The underlying file descriptor is
     * obtained the first time the object is used as an iterable (i.e.
     * when iteration starts via {@see Archive::getIterator()}).
     *
     * Because the archive is read sequentially, an Archive object can
     * only be iterated once. Attempting to iterate a second time throws
     * an {@see Exception}.
     *
     * The file path is resolved through the PHP stream layer using the
     * "rb" mode with STREAM_MUST_SEEK. This means open_basedir
     * restrictions apply and any registered stream wrapper that can cast
     * to a file descriptor can be used as the source.
     *
     * Cloning is not supported.
     *
     * @see archive_read(3) for the underlying libarchive API.
     */
    final class Archive implements \IteratorAggregate
    {
        /**
         * Create an Archive reader for the given file.
         *
         * The constructor stores the path and extraction flags but does
         * not open the file yet. Errors (file not found, permission
         * denied, …) are therefore not reported here but on first
         * iteration.
         *
         * All compression filters and archive formats supported by the
         * installed libarchive are tried automatically
         * (archive_read_support_filter_all / archive_read_support_format_all).
         *
         * @param string $file  Path to the archive file, resolved through
         *                      the PHP stream wrapper layer. open_basedir
         *                      restrictions are enforced at extraction time
         *                      (see {@see Archive::extractCurrent()}), not here.
         * @param int    $flags Bitmask of EXTRACT_* constants that controls
         *                      what metadata is restored when
         *                      {@see Archive::extractCurrent()} is called.
         *                      Defaults to 0 (restore data only).
         */
        public function __construct(string $file, int $flags = 0) {}

        /**
         * Create an Archive reader from an already-open PHP stream.
         *
         * This method does not seek to the beginning of the stream; reading
         * starts at the current position.
         *
         * The stream is cast to a C {@code FILE *} via {@code
         * php_stream_cast(PHP_STREAM_AS_STDIO)}, which generally succeeds in
         * Linux via fopencookie.
         *
         * @param resource $stream  An open, readable PHP stream.
         * @param int      $flags   Bitmask of EXTRACT_* constants forwarded to
         *                          {@see Archive::extractCurrent()}.
         */
        public static function fromStream(mixed $stream, int $flags = 0): static {}

        /**
         * Extract the current archive entry to disk.
         *
         * The entry's pathname determines where the file is written.
         * If the pathname is relative it is resolved against the current
         * working directory (prepended in-place via
         * archive_entry_set_pathname_utf8). If it is absolute,
         * open_basedir is checked and an {@see Exception} is thrown on
         * violation.
         *
         * A libarchive disk writer (archive_write_disk) is created on
         * the first call and reused for subsequent calls on the same
         * Archive instance. The extraction flags passed to the
         * constructor are forwarded to archive_write_disk_set_options.
         * Standard user/group lookup is always enabled.
         *
         * This method must be called while the iterator is positioned on
         * the same entry: calling it after the iterator has advanced, or
         * after iteration has ended, results in a libarchive internal
         * error and an {@see Exception} is thrown.
         *
         * @param Entry $entry  The current entry, as returned by the
         *                      iterator. The entry's pathname may be
         *                      modified by this method (relative paths
         *                      are made absolute in-place).
         *
         * @throws Exception    If the pathname is not available as UTF-8,
         *                      if the current directory cannot be
         *                      determined, if the resulting path is too
         *                      long, if open_basedir is violated, if the
         *                      archive is not open, if writing the file
         *                      header or data fails.
         */
        public function extractCurrent(Entry $entry): void {}

        /**
         * Return a readable PHP stream for the current entry's data.
         *
         * The stream reads directly from the libarchive decompression
         * buffer (archive_read_data). Seeking is supported only for
         * formats that implement archive_seek_data; for most formats
         * (including 7-zip as stored by default) seeking will fail with
         * a warning and fseek() will return -1.
         *
         * The stream holds a reference to the Archive object, preventing
         * it from being garbage-collected while the stream is open.
         * Writing to the stream is not supported and emits an E_WARNING.
         *
         * The stream is not automatically invalidated when the iterator
         * advances to the next entry; reading from it after that point
         * will return data from the next entry's content.
         *
         * @return resource  A PHP stream resource opened in "rb" mode.
         */
        /** @return resource */
        public function currentEntryStream(): mixed {}

        /**
         * Return an iterator over the archive entries.
         *
         * This method is called automatically by foreach. It opens the
         * archive file (via a PHP stream cast to a file descriptor) and
         * returns an internal iterator that reads one entry header at a
         * time using archive_read_next_header2.
         *
         * Each iteration step yields one {@see Entry} object. The
         * iterator does not rewind: once the archive has been opened,
         * calling getIterator() again throws an {@see Exception}.
         *
         * @return \Traversable<int, Entry>
         *
         * @throws Exception  If the file cannot be opened or cast to a
         *                    file descriptor, or if the archive format
         *                    cannot be detected, or if the archive has
         *                    already been opened.
         */
        public function getIterator(): \Traversable {}
    }
}
