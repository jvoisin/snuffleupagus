<?php

function help($name) {
	die("Usage: $name [-h|--help] [--without-hash] folder\n");
}

if ($argc < 2) {
	help($argv[0]);
}

$functions_blacklist = ['shell_exec', 'exec', 'passthru', 'php_uname', 'popen',
	'posix_kill', 'posix_mkfifo', 'posix_setpgid', 'posix_setsid', 'posix_setuid',
	'posix_setgid', 'posix_uname', 'proc_close', 'proc_nice', 'proc_open',
	'proc_terminate', 'proc_open', 'proc_get_status', 'dl', 'pnctl_exec',
	'pnctl_fork', 'assert', 'system', 'curl_exec', 'curl_multi_exec', 'function_exists'];

$extensions = ['php', 'php7', 'php5', 'inc'];

$path = realpath($argv[count($argv) - 1]);
$parsedArgs = getopt('h', ['without-hash', 'help']);

if (isset($parsedArgs['h']) || isset($parsedArgs['help'])) {
	help($argv[0]);
}

$useHash = !isset($parsedArgs['without-hash']);

$output = Array();

$objects = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path));
foreach($objects as $name => $object){
	if (FALSE === in_array (pathinfo($name, PATHINFO_EXTENSION), $extensions, true)) {
		continue;
	}

	$hash = '';
	$file_content = file_get_contents($name);

	if ($useHash) {
		$hash = '.hash("' . hash('sha256', $file_content) . '")';
	}

	foreach(token_get_all($file_content) as $token) {
		if ($token[0] !== T_STRING) {
			continue;
		}

		if (in_array($token[1], $functions_blacklist, true)) {
			$output[] = 'sp.disable_function.function("' . $token[1] . '").filename("' . $name . '")' . $hash . '.allow();' . "\n";
		}
	}
}
foreach($functions_blacklist as $fun) {
	$output[] = 'sp.disable_function.function("' . $fun . '").drop();' . "\n";
}

foreach (array_unique($output) as $line) {
 	echo $line;
}
