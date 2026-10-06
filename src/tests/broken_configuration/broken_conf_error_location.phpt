--TEST--
Configuration syntax errors retain their filename and line
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_error_location.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] parser error in %s/config/broken_conf_error_location.ini:4 in %s/config/broken_conf_error_location.ini on line 4

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/config/broken_conf_error_location.ini on line 4
Could not startup.