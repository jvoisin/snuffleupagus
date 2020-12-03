--TEST--
Broken configuration, invalid cidr for ipv6 because there is no `/` in it
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr6_no_slash.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] '2001:0db8:0000:0000:0000:ff00:0042:8329' isn't a valid network mask, it seems that you forgot a '/'. in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] '2001:0db8:0000:0000:0000:ff00:0042:8329' isn't a valid network mask, it seems that you forgot a '/'. in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
