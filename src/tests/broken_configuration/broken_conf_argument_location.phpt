--TEST--
Multiline configuration errors report the invalid argument's line
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_argument_location.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Failed to parse arg '' of `pos` in %s/config/broken_conf_argument_location.ini:5 in %s/config/broken_conf_argument_location.ini on line 5

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/config/broken_conf_argument_location.ini on line 5
Could not startup.