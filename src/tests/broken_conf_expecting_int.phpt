--TEST--
Bad integer value in configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_expecting_int.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][error][error] .mask_ipv4() is expecting a valid integer on line 2.
