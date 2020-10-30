--TEST--
Illegal states for extraction
--FILE--
<?php

include 'lib.inc';
$tmp_dir = get_temp_dir();

function get_arch() {
    return new libarchive\Archive('arch/basic.7z');
}

$a = get_arch();
foreach ($a as $e) {
    $e->pathname = "$tmp_dir/{$e->pathname}";
    $a->extractCurrent($e);
}
try {
    $a->extractCurrent($e);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
==DONE==
--EXPECT--
Error reading data block: INTERNAL ERROR: Function 'archive_read_data_block' invoked with archive structure in state 'eof', should be in state 'data'
==DONE==
