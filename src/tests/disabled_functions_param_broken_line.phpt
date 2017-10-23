--TEST--
Disable functions - match on a specific line - broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_broken_line.ini
--FILE--
<?php 
system("echo 1337");
system("echo 1338");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][config][error] Failed to parse arg 'qwe' of `line` on line 1.
1337
1338
