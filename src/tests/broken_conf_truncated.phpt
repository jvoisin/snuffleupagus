--TEST--
Bad boolean value in configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_broken_conf_truncated.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][error] A valid string as parameter is expected on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][error] A valid string as parameter is expected on line 1 in Unknown on line 0