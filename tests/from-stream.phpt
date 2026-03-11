--TEST--
Archive::fromStream() opens from an existing PHP stream resource
--FILE--
<?php

include 'lib.inc';

echo "-- fopen'd file --\n";
$f = fopen(__DIR__ . '/arch/basic.7z', 'rb');
foreach (libarchive\Archive::fromStream($f) as $e) {
    echo $e->pathname, ' ', $e->size, "\n";
}
fclose($f);

echo "-- php://memory (non-fd stream) --\n";
$mem = fopen('php://memory', 'rb+');
fwrite($mem, file_get_contents(__DIR__ . '/arch/basic.7z'));
rewind($mem);
foreach (libarchive\Archive::fromStream($mem) as $e) {
    echo $e->pathname, ' ', $e->size, "\n";
}
fclose($mem);

echo "-- current position is honoured (stream at EOF) --\n";
$f = fopen(__DIR__ . '/arch/basic.7z', 'rb');
fseek($f, 0, SEEK_END);
try {
    foreach (libarchive\Archive::fromStream($f) as $e) {
        echo $e->pathname, "\n";
    }
    echo "no entries\n";
} catch (libarchive\Exception $e) {
    echo "Exception caught\n";
}
fclose($f);

?>
--EXPECT--
-- fopen'd file --
myfile.txt 35
-- php://memory (non-fd stream) --
myfile.txt 35
-- current position is honoured (stream at EOF) --
no entries
