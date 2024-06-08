--TEST--
Configuration line without closing parentheses
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_line_no_closing.ini
error_log=/dev/null
--FILE--
--EXPECT--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Missing argument to keyword 'name' - it should be 'name("...")' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
