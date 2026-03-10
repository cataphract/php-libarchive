<?php

function get_temp_dir() {
	static $dir;
	if (!$dir) {
		$dir = sys_get_temp_dir() . DIRECTORY_SEPARATOR
			. 'phpla_' . getmypid() . '_' . uniqid();
		if (!mkdir($dir, 0700)) {
			exit("Could not create temp dir: $dir\n");
		}

		register_shutdown_function(function() use ($dir) {
			$files = new RecursiveIteratorIterator(
				new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS),
				RecursiveIteratorIterator::CHILD_FIRST
			);

			foreach ($files as $fileinfo) {
				if ($fileinfo->isLink() || !$fileinfo->isDir()) {
					$todo = 'unlink';
				} else {
					$todo = 'rmdir';
				}

				$todo($fileinfo->getRealPath());
			}

			rmdir($dir);
		});
	}
	return $dir;
}

