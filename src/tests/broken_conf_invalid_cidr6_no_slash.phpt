--TEST--
Broken configuration, invalid cidr for ipv6 because there is no `/` in it
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr6_no_slash.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] '2001:0db8:0000:0000:0000:ff00:0042:8329' isn't a valid network mask, it seems that you forgot a '/'.
