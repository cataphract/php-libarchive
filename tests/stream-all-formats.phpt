--TEST--
Stream extraction of all supported archive formats
--FILE--
<?php

include 'lib.inc';

$archives = glob(__DIR__ . '/archives/*');
sort($archives);

// lrzip and lzo require external tools (lrzip/lzop) that emit progress text
$skip = ['test.tar.lrzip', 'test.tar.lzo'];
$skip = array_merge($skip, ['test.ar']); // not supported on windows

foreach ($archives as $path) {
    $name = basename($path);
    if (in_array($name, $skip, true)) {
        continue;
    }
    try {
        $a = new libarchive\Archive($path);
        foreach ($a as $e) {
            $content = stream_get_contents($a->currentEntryStream());
            if ($content === '') {
                continue; // skip entries with no data (e.g. directories in ISO)
            }
            echo "$name: $content";
        }
    } catch (libarchive\Exception $ex) {
        echo "$name: ERROR: " . $ex->getMessage() . "\n";
    }
}
?>
--EXPECT--
test-bin.cpio: Hello, World!
test-gnu.tar: Hello, World!
test-odc.cpio: Hello, World!
test-pax.tar: Hello, World!
test-stored.zip: Hello, World!
test-ustar.tar: Hello, World!
test.7z: Hello, World!
test.cab: Hello, World!
test.cpio: Hello, World!
test.iso: Hello, World!
test.rpm: Hello, World!
test.tar.Z: Hello, World!
test.tar.bz2: Hello, World!
test.tar.gz: Hello, World!
test.tar.lz: Hello, World!
test.tar.lz4: Hello, World!
test.tar.lzma: Hello, World!
test.tar.uu: Hello, World!
test.tar.xz: Hello, World!
test.tar.zst: Hello, World!
test.warc: Hello, World!
test.xar: Hello, World!
test.zip: Hello, World!
