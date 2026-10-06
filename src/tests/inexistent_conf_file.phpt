--TEST--
Check for snuffleupagus presence
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/unexistent_configuration_file.ini
error_log=/dev/null
--FILE--
<?php ?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Could not open configuration file %a/config/unexistent_configuration_file.ini : No such file or directory in %s/config/unexistent_configuration_file.ini on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/config/unexistent_configuration_file.ini on line 0
Could not startup.
