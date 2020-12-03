--TEST--
Broken configuration - enabled/disabled unserialize
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_mutually_exclusive9.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] A rule can't be enabled and disabled on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] A rule can't be enabled and disabled on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
