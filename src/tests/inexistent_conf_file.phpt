--TEST--
Check for snuffleupagus presence
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/unexistent_configuration_file.ini
--FILE--
<?php ?>
--EXPECTF--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config] Could not open configuration file %a/config/unexistent_configuration_file.ini : No such file or directory in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Could not open configuration file %a/config/unexistent_configuration_file.ini : No such file or directory in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Invalid configuration file in Unknown on line 0
Could not startup.