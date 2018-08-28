--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_config_regexp.ini
--FILE--
--EXPECTF--
PHP Fatal error:  [snuffleupagus][config] Failed to compile '*.': %s on line 1. in Unknown on line 0
PHP Fatal error:  [snuffleupagus][config] '.filename_r()' is expecting a valid regexp, and not '"*."' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Failed to compile '*.': %s on line 1. in Unknown on line 0

Fatal error: [snuffleupagus][config] '.filename_r()' is expecting a valid regexp, and not '"*."' on line 1 in Unknown on line 0
