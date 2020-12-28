--TEST--
Broken configuration - regexp without a closing parenthesis
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_config_regexp_no_closing_paren.ini
--FILE--
--EXPECT--

Fatal error: [snuffleupagus][0.0.0.0][error][log] There is an issue with the parsing of '"*."': it doesn't look like a valid string on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] '.filename_r()' is expecting a valid regexp, and not '"*."' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
