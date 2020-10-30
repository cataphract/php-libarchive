--TEST--
Simple 7zip extraction
--FILE--
<?php

include 'lib.inc';
$tmp_dir = get_temp_dir();
$a = new libarchive\Archive('arch/basic.7z');
foreach ($a as $e) {
    $e->pathname = "$tmp_dir/{$e->pathname}";
    $a->extractCurrent($e);
}

foreach (new DirectoryIterator($tmp_dir) as $f) {
    if ($f->isDot()) continue;
    echo $f->getFilename(), ' ', $f->getSize(), "\n";
}

?>
--EXPECT--
myfile.txt 35
