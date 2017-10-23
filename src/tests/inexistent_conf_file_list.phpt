--TEST--
Non-existent configuration file in a list
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../config/default.ini,{PWD}/non_existent_configuration_file
--FILE--
<?php ?>
--EXPECTF--
[snuffleupagus][0.0.0.0][config][error] Could not open configuration file%a/non_existent_configuration_file : No such file or directory
