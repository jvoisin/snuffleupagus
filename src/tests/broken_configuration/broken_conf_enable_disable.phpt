--TEST--
Global strict mode
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/borken_conf_enable_disable.ini
--FILE--
--EXPECTF--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config] A rule can't be enabled and disabled on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] A rule can't be enabled and disabled on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Invalid configuration file in Unknown on line 0
Could not startup.