--TEST--
Dump request - invalid folder.
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) { print "skip"; } 
?>
--POST--
post_a=data_post_a&post_b=data_post_b
--GET--
get_a=data_get_a&get_b=data_get_b
--COOKIE--
cookie_a=data_cookie_a&cookie_b=data_cookie_b
--INI--
sp.configuration_file={PWD}/config/dump_request_invalid_folder.ini
--FILE--
<?php
echo "1\n";
echo system("echo 1337;");
echo "2\n";
?>
--EXPECTF--
1

Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'system' in %a/src/tests/dump_request_invalid_folder.php on line 3