--TEST--
Broken regexp
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_regexp.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Failed to compile '^$[': missing terminating ] for character class. in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid regexp '^$[' for '.value_r()' in %s/tests/broken_configuration/config/broken_regexp.ini:1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
