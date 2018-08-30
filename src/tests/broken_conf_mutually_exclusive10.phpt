--TEST--
Broken configuration - enabled/disabled readonly
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_mutually_exclusive10.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] A rule can't be enabled and disabled on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] A rule can't be enabled and disabled on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.