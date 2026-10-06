--TEST--
Non-existent configuration file in a list
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../config/default.rules,{PWD}/non_existent_configuration_file
error_log=/dev/null
--FILE--
<?php ?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Could not open configuration file %a/non_existent_configuration_file : No such file or directory in %s/non_existent_configuration_file on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/non_existent_configuration_file on line 0
Could not startup.
