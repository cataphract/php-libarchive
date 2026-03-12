--TEST--
withPasswordCallback() supplies a passphrase for encrypted archives
--FILE--
<?php

echo "-- callback is invoked when reading data --\n";
$called = 0;
$a = (new libarchive\Archive('arch/encrypted/test_encrypted.zip'))
    ->withPasswordCallback(function() use (&$called) {
        $called++;
        return 'password';
    });
foreach ($a as $e) {
    if ($e->size) {
    $content = trim(stream_get_contents($a->currentEntryStream()));
    echo $e->pathname, ' ', $content, "\n";
    }
}
echo "callback invoked: ", $called, " time(s)\n";

echo "-- wrong password, give up after 10 attempts --\n";
$called = 0;
$a = (new libarchive\Archive('arch/encrypted/test_encrypted.zip'))
    ->withPasswordCallback(function() use (&$called) {
        $called++;
        return $called <= 10 ? 'wrong' : null;
    });
foreach ($a as $e) {
    if (!$e->size) continue;
    $content = stream_get_contents($a->currentEntryStream());
    echo $e->pathname, ' len=', strlen($content), "\n";
}
echo "callback invoked: ", $called, " time(s)\n";

echo "-- withPasswordCallback after open --\n";
$a = new libarchive\Archive('arch/encrypted/test_encrypted.zip');
foreach ($a as $e) { break; }
try {
    $a->withPasswordCallback(fn() => 'password');
} catch (libarchive\Exception $e) {
    echo "Exception: ", $e->getMessage(), "\n";
}

?>
--EXPECTF--
-- callback is invoked when reading data --
encrypted/file.txt plaintext
callback invoked: 1 time(s)
-- wrong password, give up after 10 attempts --

Warning: stream_get_contents(): Error reading data: Incorrect passphrase [-1] in %s on line %d
encrypted/file.txt len=0
callback invoked: 11 time(s)
-- withPasswordCallback after open --
Exception: Cannot set password callback after archive has been opened
