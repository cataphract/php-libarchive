<?php

function get_temp_dir() {
	static $dir;
	if (!$dir) {
		$dir = tempnam(__DIR__, '.tmp');
		unlink($dir);
		mkdir($dir);

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

