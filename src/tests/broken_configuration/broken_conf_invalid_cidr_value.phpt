--TEST--
Broken configuration, invalid cidr value
(13337%128 = 25)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr_value.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Missing argument to keyword 'cidr' - it should be 'cidr("...")' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Missing argument to keyword 'cidr' - it should be 'cidr("...")' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
