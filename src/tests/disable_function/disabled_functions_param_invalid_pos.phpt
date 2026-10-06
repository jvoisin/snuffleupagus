--TEST--
Disable functions - match on argument's position
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_invalid_pos.ini
error_log=/dev/null
--FILE--
<?php 
system("echo 1");
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Failed to parse arg 'qwe' of `pos` in %s/tests/disable_function/config/disabled_functions_invalid_pos.ini:1 in %s/disable_function/config/disabled_functions_invalid_pos.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/disable_function/config/disabled_functions_invalid_pos.ini on line 1
Could not startup.
