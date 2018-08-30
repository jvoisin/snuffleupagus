--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] '42' isn't a valid ipv4 mask. in Unknown on line 0

Fatal error: [snuffleupagus][config] '42' isn't a valid ipv4 mask. in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.