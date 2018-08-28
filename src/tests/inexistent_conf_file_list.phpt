--TEST--
Non-existent configuration file in a list
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../config/default.rules,{PWD}/non_existent_configuration_file
--FILE--
<?php ?>
--EXPECTF--
PHP Fatal error:  [snuffleupagus][config] Could not open configuration file %a/non_existent_configuration_file : No such file or directory in Unknown on line 0

Fatal error: [snuffleupagus][config] Could not open configuration file %a/non_existent_configuration_file : No such file or directory in Unknown on line 0