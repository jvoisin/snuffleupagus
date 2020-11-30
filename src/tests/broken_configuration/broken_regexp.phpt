--TEST--
Broken regexp
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_regexp.ini
--FILE--
--EXPECTF--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Failed to compile '^$[': missing terminating ] for character class on line 1. in Unknown on line 0
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] '.value_r()' is expecting a valid regexp, and not '"^$["' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Failed to compile '^$[': missing terminating ] for character class on line 1. in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] '.value_r()' is expecting a valid regexp, and not '"^$["' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.