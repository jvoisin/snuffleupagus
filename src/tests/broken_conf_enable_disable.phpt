--TEST--
Global strict mode
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/borken_conf_enable_disable.ini
--FILE--
--EXPECTF--
[snuffleupagus][0.0.0.0][config][error] A rule can't be enabled and disabled.
