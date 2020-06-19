--TEST--
Dump request - nonwriteable folder.
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) { print "skip"; } 
if ("ubuntu" == getenv("CI_JOB_IMAGE")) { print "skip"; }

// root has write privileges "any" folders
if (TRUE == function_exists("posix_getuid")) {
	if (0 == posix_getuid()) {
		print "skip";
	}
} elseif (TRUE == function_exists("shell_exec")) {
	if ("root" == trim(shell_exec("whoami"))) {
		print "skip";
	}
}

?>
--POST--
post_a=data_post_a&post_b=data_post_b
--GET--
get_a=data_get_a&get_b=data_get_b
--COOKIE--
cookie_a=data_cookie_a&cookie_b=data_cookie_b
--INI--
sp.configuration_file={PWD}/config/dump_request_nonwriteable_folder.ini
--FILE--
<?php
echo "1\n";
echo system("echo 1337;");
echo "2\n";
?>
--EXPECTF--
1

Warning: [snuffleupagus][0.0.0.0][request_logging] Unable to open %a: Permission denied in %a/dump_request_nonwriteable_folder.php on line %d

Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'system' in %a/dump_request_nonwriteable_folder.php on line 3
