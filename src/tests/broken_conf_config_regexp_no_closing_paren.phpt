--TEST--
Broken configuration - regexp without a closing parenthesis
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_config_regexp_no_closing_paren.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][error] There is an issue with the parsing of '"*."': it doesn't look like a valid string on line 1 in Unknown on line 0
PHP Fatal error:  [snuffleupagus][config] '.filename_r()' is expecting a valid regexp, and not '"*."' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][error] There is an issue with the parsing of '"*."': it doesn't look like a valid string on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] '.filename_r()' is expecting a valid regexp, and not '"*."' on line 1 in Unknown on line 0