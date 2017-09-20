--TEST--
Empty configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/empty_conf.ini
--FILE--
--EXPECT--
