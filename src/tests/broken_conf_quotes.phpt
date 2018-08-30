--TEST--
Broken configuration - missing quote
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_quotes.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] You forgot to close a bracket. in Unknown on line 0
PHP Fatal error:  [snuffleupagus][config] Invalid value '_SERVER[PHP_SELF' for `var` on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] You forgot to close a bracket. in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid value '_SERVER[PHP_SELF' for `var` on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.