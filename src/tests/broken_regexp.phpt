--TEST--
Broken regexp
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_regexp.ini
--FILE--
--EXPECTF--
[snuffleupagus][0.0.0.0][config][error] Failed to compile '^$[': missing terminating ] for character class on line 1.
[snuffleupagus][0.0.0.0][config][error] '.value_r()' is expecting a valid regexp, and not '"^$["' on line 1.
