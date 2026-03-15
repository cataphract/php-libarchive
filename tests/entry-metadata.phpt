--TEST--
Entry metadata properties
--FILE--
<?php

$a = new libarchive\Archive('arch/metadata.tar.gz');
foreach ($a as $e) {
    echo "--- {$e->pathname} ---\n";
    echo "type=", $e->type, "\n";
    echo "isFile=", var_export($e->isFile, true), "\n";
    echo "isDir=", var_export($e->isDir, true), "\n";
    echo "isSymlink=", var_export($e->isSymlink, true), "\n";
    echo "perm=", decoct($e->perm), "\n";
    echo "uid=", $e->uid, "\n";
    echo "gid=", $e->gid, "\n";
    echo "uname=", $e->uname ?? "NULL", "\n";
    echo "gname=", $e->gname ?? "NULL", "\n";
    echo "size=", $e->size ?? "NULL", "\n";
    echo "mtime=", $e->mtime ?? "NULL", "\n";
    echo "atime=", $e->atime ?? "NULL", "\n";
    echo "ctime=", $e->ctime ?? "NULL", "\n";
    echo "symlink=", $e->symlink ?? "NULL", "\n";
    echo "hardlink=", $e->hardlink ?? "NULL", "\n";
    echo "rdev=", $e->rdev, "\n";
    echo "isEncrypted=", var_export($e->isEncrypted, true), "\n";
    echo "\n";
}

// Verify type constants match
var_dump(libarchive\ENTRY_TYPE_FILE === 0100000);
var_dump(libarchive\ENTRY_TYPE_DIR === 0040000);
var_dump(libarchive\ENTRY_TYPE_SYMLINK === 0120000);
var_dump(libarchive\ENTRY_TYPE_FIFO === 0010000);

?>
--EXPECT--
--- hello.txt ---
type=32768
isFile=true
isDir=false
isSymlink=false
perm=644
uid=1000
gid=500
uname=testuser
gname=testgroup
size=13
mtime=1700000000
atime=1700000001
ctime=1700000002
symlink=NULL
hardlink=NULL
rdev=0
isEncrypted=false

--- mydir/ ---
type=16384
isFile=false
isDir=true
isSymlink=false
perm=755
uid=0
gid=0
uname=root
gname=root
size=0
mtime=1700000100
atime=NULL
ctime=NULL
symlink=NULL
hardlink=NULL
rdev=0
isEncrypted=false

--- link.txt ---
type=40960
isFile=false
isDir=false
isSymlink=true
perm=777
uid=1000
gid=500
uname=testuser
gname=testgroup
size=0
mtime=1700000200
atime=NULL
ctime=NULL
symlink=hello.txt
hardlink=NULL
rdev=0
isEncrypted=false

--- hardlink.txt ---
type=0
isFile=false
isDir=false
isSymlink=false
perm=644
uid=1000
gid=500
uname=testuser
gname=testgroup
size=0
mtime=1700000300
atime=NULL
ctime=NULL
symlink=NULL
hardlink=hello.txt
rdev=0
isEncrypted=false

--- myfifo ---
type=4096
isFile=false
isDir=false
isSymlink=false
perm=644
uid=1000
gid=500
uname=testuser
gname=testgroup
size=0
mtime=1700000400
atime=NULL
ctime=NULL
symlink=NULL
hardlink=NULL
rdev=0
isEncrypted=false

--- mychardev ---
type=8192
isFile=false
isDir=false
isSymlink=false
perm=666
uid=0
gid=0
uname=root
gname=root
size=0
mtime=1700000500
atime=NULL
ctime=NULL
symlink=NULL
hardlink=NULL
rdev=259
isEncrypted=false

bool(true)
bool(true)
bool(true)
bool(true)
