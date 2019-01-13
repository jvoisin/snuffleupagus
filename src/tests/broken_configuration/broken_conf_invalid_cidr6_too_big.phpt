--TEST--
Broken configuration, cidr for ipv6 is too big, that will `mod` to 25.
(13337%128 = 25)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr6_too_big.ini
--FILE--
--EXPECT--
