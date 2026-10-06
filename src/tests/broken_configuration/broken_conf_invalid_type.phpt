--TEST--
Broken conf with wrong type
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_type.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Missing argument to keyword 'ret_type' - it should be 'ret_type("...")' on line 1 in %s/broken_configuration/config/broken_conf_invalid_type.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_invalid_type.ini on line 1
Could not startup.
