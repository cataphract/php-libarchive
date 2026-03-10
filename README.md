# php-libarchive

PHP extension for reading archives (tar, zip, 7-zip, cpio, …) via [libarchive](https://libarchive.org/). Supports all compression filters the installed libarchive recognises (gzip, bzip2, xz, zstd, …).

## Requirements

- PHP 8.0+
- libarchive 3.x

## Installation

```bash
pie install cataphract/libarchive
```

## Usage

### Extract an archive to disk

```php
use libarchive\Archive;
use libarchive\EXTRACT_PERM;
use libarchive\EXTRACT_TIME;

$archive = new Archive('/path/to/archive.tar.gz', EXTRACT_PERM | EXTRACT_TIME);

chdir('/tmp/extract');

foreach ($archive as $entry) {
    echo $entry->pathname . "\n";
    $archive->extractCurrent($entry);
}
```

### Read an entry as a stream

```php
use libarchive\Archive;

$archive = new Archive('/path/to/archive.zip');

foreach ($archive as $entry) {
    if ($entry->pathname === 'data.json') {
        $stream = $archive->currentEntryStream();
        $contents = stream_get_contents($stream);
        fclose($stream);
        break;
    }
}
```

## API

See [`libarchive.stub.php`](libarchive.stub.php) for the full class and constant documentation.

## License

MIT
