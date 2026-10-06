--TEST--
Nested conditions are rejected even when the outer condition is false
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_nested_condition_false.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] nested conditions are not supported in %s/tests/broken_configuration/config/broken_conf_nested_condition_false.ini:2 in %s/broken_configuration/config/broken_conf_nested_condition_false.ini on line 2

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_nested_condition_false.ini on line 2
Could not startup.