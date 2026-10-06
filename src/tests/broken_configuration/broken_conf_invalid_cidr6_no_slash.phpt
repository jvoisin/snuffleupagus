--TEST--
Broken configuration, invalid cidr for ipv6 because there is no `/` in it
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr6_no_slash.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] '2001:0db8:0000:0000:0000:ff00:0042:8329' isn't a valid network mask, it seems that you forgot a '/'. in %s/broken_configuration/config/broken_conf_invalid_cidr6_no_slash.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_invalid_cidr6_no_slash.ini on line 1
Could not startup.
