--TEST--
Broken regexp
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_regexp.ini
--FILE--
--EXPECTF--
PHP Fatal error:  [snuffleupagus][config] Failed to compile '^$[': missing terminating ] for character class on line 1. in Unknown on line 0
PHP Fatal error:  [snuffleupagus][config] '.value_r()' is expecting a valid regexp, and not '"^$["' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Failed to compile '^$[': missing terminating ] for character class on line 1. in Unknown on line 0

Fatal error: [snuffleupagus][config] '.value_r()' is expecting a valid regexp, and not '"^$["' on line 1 in Unknown on line 0