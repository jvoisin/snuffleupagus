--TEST--
Configuration line with an empty string
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_line_empty_string.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][error] A valid string as parameter is expected on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][error] A valid string as parameter is expected on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Invalid configuration file in Unknown on line 0
Could not startup.