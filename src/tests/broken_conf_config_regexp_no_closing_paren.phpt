--TEST--
Broken configuration - regexp without a closing parenthesis
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_config_regexp_no_closing_paren.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][error][error] There is an issue with the parsing of '"*."': it doesn't look like a valid string on line 1.
[snuffleupagus][0.0.0.0][config][error] '.filename_r()' is expecting a valid regexp, and not '"*."' on line 1.
