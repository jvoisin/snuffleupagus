--TEST--
Broken configuration - enabled/disabled unserialize
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_mutually_exclusive9.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] A rule can't be enabled and disabled on line 1.
