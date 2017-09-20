--TEST--
Check for snuffleupagus presence
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/unexistent_configuration_file.ini
--FILE--
<?php ?>
--EXPECTF--
[snuffleupagus][0.0.0.0][config][error] Could not open configuration file %a/tests/config/unexistent_configuration_file.ini : No such file or directory
