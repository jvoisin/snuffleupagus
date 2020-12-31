--TEST--
Test for unexistent configuration file, in php8
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/unexistent_configuration_file.ini
--FILE--
<?php ?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Could not open configuration file %a/config/unexistent_configuration_file.ini : No such file or directory in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
