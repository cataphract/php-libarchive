--TEST--
Stream is invalidated when iterator advances to the next entry
--FILE--
<?php

include 'lib.inc';

$a = new libarchive\Archive('arch/basic.7z');
$it = new IteratorIterator($a);
$it->rewind();
$e = $it->current();

$stream = $a->currentEntryStream();

// advance to next entry: next() marks current as invalid,
// valid() triggers the lazy archive_read_next_header2 call
$it->next();
$it->valid();

// reading from the old stream should now warn and return nothing
$data = fread($stream, 1024);
var_dump($data);

?>
==DONE==
--EXPECTF--
Warning: fread(): Stream is no longer valid: the archive has moved to the next entry in %s on line %d
bool(false)
==DONE==
