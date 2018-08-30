--TEST--
Configuration line without closing parenthese
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_line_no_closing.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][error] There is an issue with the parsing of '"123"': it doesn't look like a valid string on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][error] There is an issue with the parsing of '"123"': it doesn't look like a valid string on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.