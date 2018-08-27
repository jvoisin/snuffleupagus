--TEST--
Configuration line with an empty string
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_line_empty_string.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][error] A valid string as parameter is expected on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][error] A valid string as parameter is expected on line 1 in Unknown on line 0