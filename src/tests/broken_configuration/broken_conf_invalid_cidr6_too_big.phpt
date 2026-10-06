--TEST--
Broken configuration, cidr for ipv6 is too big, that will `mod` to 25.
(13337%128 = 25)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr6_too_big.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] '13337' isn't a valid network mask. in %s/broken_configuration/config/broken_conf_invalid_cidr6_too_big.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_invalid_cidr6_too_big.ini on line 1
Could not startup.