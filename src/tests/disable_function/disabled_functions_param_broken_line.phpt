--TEST--
Disable functions - match on a specific line - broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_broken_line.ini
--FILE--
<?php 
system("echo 1337");
system("echo 1338");
?>
--EXPECTF--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Failed to parse arg 'qwe' of `line` on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Failed to parse arg 'qwe' of `line` on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
