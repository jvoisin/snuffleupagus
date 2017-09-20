--TEST--
Broken configuration, invalid cidr value
(13337%128 = 25)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr_value.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][error][error] There is an issue with the parsing of '"': it doesn't look like a valid string.
[snuffleupagus][0.0.0.0][config][error] " doesn't contain a valid cidr.
