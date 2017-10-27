--TEST--
Bad boolean value in configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_broken_conf_truncated.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][error][error] A valid string as parameter is expected on line 1.
