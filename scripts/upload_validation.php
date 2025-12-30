#!/usr/bin/env php
<?php

function check($filename) {

	$whitelist = ['ECHO', 'RETURN', 'PHP', 'NOP'];

	$out = [];
	$ret = 0;
	$cmd = [
		PHP_BINARY,
		"-d", "vld.active=1",
		"-d", "vld.execute=0",
		"-d", "extension=vld.so",
		"-d", "vld.format=1",
		"-d", "vld.col_sep=@",
		"-d", "log_errors=0",
		"-d", "error_log=/dev/null",
            	"-B", "if (!extension_loaded('vld')) die('no vld');",
		escapeshellarg($filename),
		'2>&1',
		];
	exec(implode(' ', $cmd), $out, $ret);
	if ($ret) {
        printf("Error: %d\n", $ret);
		return 2;
	}
	foreach($out as $line) {
		$sp = explode('@', $line);
		if (count($sp) < 5) {
			continue;
		}
		$opcode = $sp[4]; // # ,line, #, EIO, op, fetch, ext, return, operands
		if ($opcode && !in_array($opcode, $whitelist)) {
			printf("Upload_validation: Found an opcode: %s\n", $opcode);
			return 1;
		}
	}
	return 0;
}

if ($_SERVER['argc'] != 2) {
	die("Usage: {$_SERVER['argv']['0']} file_to_test.php\n");
}
exit(check($_SERVER['argv']['1']));

