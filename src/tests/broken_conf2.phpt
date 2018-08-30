--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf2.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] Invalid configuration section 'sp.wrong' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration section 'sp.wrong' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.