--TEST--
Entry get_properties returns all properties
--FILE--
<?php

$a = new libarchive\Archive('arch/metadata.tar.gz');
$expected_keys = ['atime', 'ctime', 'gid', 'gname', 'hardlink', 'isDir', 'isEncrypted',
                  'isFile', 'isSymlink', 'mtime', 'pathname', 'perm', 'rdev', 'size',
                  'symlink', 'type', 'uid', 'uname'];

foreach ($a as $i => $e) {
    $props = (array)$e;
    $keys = array_keys($props);
    sort($keys);
    var_dump($keys === $expected_keys);

    if ($i === 0) {
        ksort($props);
        foreach ($props as $k => $v) {
            if (isset($v) && !isset($e->$k)) {
                die("Unexpected value of has_property");
            }
            echo "$k=", var_export($v, true), "\n";
        }
    }
}
?>
--EXPECT--
bool(true)
atime=1700000001
ctime=1700000002
gid=500
gname='testgroup'
hardlink=NULL
isDir=false
isEncrypted=false
isFile=true
isSymlink=false
mtime=1700000000
pathname='hello.txt'
perm=420
rdev=0
size=13
symlink=NULL
type=32768
uid=1000
uname='testuser'
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
