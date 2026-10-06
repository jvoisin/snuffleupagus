--TEST--
Broken configuration, invalid cidr value
(13337%128 = 25)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr_value.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Missing argument to keyword 'cidr' - it should be 'cidr("...")' on line 1 in %s/broken_configuration/config/broken_conf_invalid_cidr_value.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_invalid_cidr_value.ini on line 1
Could not startup.
