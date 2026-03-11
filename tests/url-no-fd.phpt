--TEST--
Constructor falls back to FILE* for stream wrappers that cannot be cast to a file descriptor
--FILE--
<?php

/* A stream wrapper that delegates reads/seeks to a real file but never
 * exposes a file descriptor (no stream_cast method). */
class NoFdWrapper {
    private $fp;

    public function stream_open(string $path, string $mode,
                                int $options, ?string &$opened_path): bool {
        $file = substr($path, strlen('nofd://'));
        $this->fp = fopen($file, $mode);
        return $this->fp !== false;
    }

    public function stream_read(int $count): string|false {
        return fread($this->fp, $count);
    }

    public function stream_seek(int $offset, int $whence): bool {
        return fseek($this->fp, $offset, $whence) === 0;
    }

    public function stream_tell(): int {
        return ftell($this->fp);
    }

    public function stream_eof(): bool {
        return feof($this->fp);
    }

    public function stream_close(): void {
        fclose($this->fp);
    }

    public function stream_cast(int $cast_as): mixed {
        return false;
    }
}

stream_wrapper_register('nofd', NoFdWrapper::class);

$a = new libarchive\Archive('nofd://' . __DIR__ . '/arch/basic.7z');
foreach ($a as $e) {
    echo $e->pathname, ' ', $e->size, "\n";
}

?>
--EXPECT--
myfile.txt 35
