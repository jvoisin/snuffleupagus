--TEST--
Global strict mode
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../broken_configuration/config/broken_conf_enable_disable2.ini
--FILE--
--EXPECTF--
