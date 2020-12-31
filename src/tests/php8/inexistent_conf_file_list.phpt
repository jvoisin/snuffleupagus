--TEST--
Non-existent configuration file in a list in php8
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../../config/default.rules,{PWD}/non_existent_configuration_file
--FILE--
<?php ?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Could not open configuration file %a/non_existent_configuration_file : No such file or directory in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
