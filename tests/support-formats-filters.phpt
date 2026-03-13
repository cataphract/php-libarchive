--TEST--
supportFormats() and supportFilters() restrict archive format and filter detection
--SKIPIF--
<?php
if (!extension_loaded('archive')) die('skip archive extension not loaded');
if (\libarchive\LIBARCHIVE_VERSION_ID < 3005000) die('skip requires libarchive >= 3.5.0');
?>
--FILE--
<?php

include 'lib.inc';

echo "-- explicit correct format --\n";
foreach ((new libarchive\Archive('arch/basic.7z'))->supportFormats(libarchive\FORMAT_7ZIP) as $e) {
    echo $e->pathname, ' ', $e->size, "\n";
}

echo "-- explicit correct filter --\n";
foreach ((new libarchive\Archive('arch/basic.7z'))->supportFilters(libarchive\FILTER_NONE) as $e) {
    echo $e->pathname, ' ', $e->size, "\n";
}

echo "-- both explicit, chained --\n";
foreach ((new libarchive\Archive('arch/basic.7z'))
        ->supportFormats(libarchive\FORMAT_7ZIP)
        ->supportFilters(libarchive\FILTER_NONE) as $e) {
    echo $e->pathname, ' ', $e->size, "\n";
}

echo "-- multiple formats, one matching --\n";
foreach ((new libarchive\Archive('arch/basic.7z'))
        ->supportFormats(libarchive\FORMAT_TAR, libarchive\FORMAT_7ZIP) as $e) {
    echo $e->pathname, ' ', $e->size, "\n";
}

echo "-- multiple filters, one matching --\n";
foreach ((new libarchive\Archive('arch/basic.7z'))
        ->supportFilters(libarchive\FILTER_GZIP, libarchive\FILTER_NONE) as $e) {
    echo $e->pathname, ' ', $e->size, "\n";
}

echo "-- wrong format --\n";
try {
    foreach ((new libarchive\Archive('arch/basic.7z'))->supportFormats(libarchive\FORMAT_TAR) as $e) {
        echo $e->pathname, "\n";
    }
} catch (libarchive\Exception $e) {
    echo "Exception: ", $e->getMessage(), "\n";
}

echo "-- supportFormats after open --\n";
$a = new libarchive\Archive('arch/basic.7z');
foreach ($a as $e) { break; }
try {
    $a->supportFormats(libarchive\FORMAT_7ZIP);
} catch (libarchive\Exception $e) {
    echo "Exception: ", $e->getMessage(), "\n";
}

echo "-- supportFilters after open --\n";
$a = new libarchive\Archive('arch/basic.7z');
foreach ($a as $e) { break; }
try {
    $a->supportFilters(libarchive\FILTER_NONE);
} catch (libarchive\Exception $e) {
    echo "Exception: ", $e->getMessage(), "\n";
}

?>
--EXPECT--
-- explicit correct format --
myfile.txt 35
-- explicit correct filter --
myfile.txt 35
-- both explicit, chained --
myfile.txt 35
-- multiple formats, one matching --
myfile.txt 35
-- multiple filters, one matching --
myfile.txt 35
-- wrong format --
Exception: Could not open archive from file descriptor: Unrecognized archive format
-- supportFormats after open --
Exception: Cannot change format after archive has been opened
-- supportFilters after open --
Exception: Cannot change filter after archive has been opened
