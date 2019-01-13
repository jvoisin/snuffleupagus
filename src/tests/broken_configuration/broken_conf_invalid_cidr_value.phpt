--TEST--
Broken configuration, invalid cidr value
(13337%128 = 25)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr_value.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][error] A valid string as parameter is expected on line 1 in Unknown on line 0
PHP Fatal error:  [snuffleupagus][config] " doesn't contain a valid cidr on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][error] A valid string as parameter is expected on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] " doesn't contain a valid cidr on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.