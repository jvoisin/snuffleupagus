--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr6.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] 'ZZZ' isn't a valid network mask.
