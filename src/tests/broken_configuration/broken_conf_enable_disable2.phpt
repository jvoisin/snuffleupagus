--TEST--
Global strict mode
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../broken_configuration/config/broken_conf_enable_disable2.ini
--FILE--
--EXPECTF--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] A rule can't be enabled and disabled on line 2 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] A rule can't be enabled and disabled on line 2 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
