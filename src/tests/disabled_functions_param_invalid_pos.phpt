--TEST--
Disable functions - match on argument's position
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_invalid_pos.ini
--FILE--
<?php 
system("echo 1");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][config][error] Failed to parse arg 'qwe' of `pos` on line 1.
1

