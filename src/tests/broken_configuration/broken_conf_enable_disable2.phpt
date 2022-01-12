--TEST--
Global strict mode
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../broken_configuration/config/broken_conf_enable_disable2.ini
error_log=/dev/null
--FILE--
--EXPECTF--
