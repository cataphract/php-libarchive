--TEST--
Basic stream functionality
--FILE--
<?php

include 'lib.inc';

function getStream() {
    $a = new libarchive\Archive('arch/basic.7z');
    $it = new IteratorIterator($a);
	$it->next();
    $e = $it->current();
    return $a->currentEntryStream();
}

$s = getStream();
echo stream_get_contents($s), "\n";
var_dump(fseek($s, 0));
echo stream_get_contents($s), "\n";

?>
==DONE==
--EXPECTF--
These are the contents of my file.


Warning: fseek(): Error seeking in entry: Internal error: No format_seek_data_block function registered [22] in %s on line %d
int(-1)

==DONE==
