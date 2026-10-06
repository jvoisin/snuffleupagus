--TEST--
Bad boolean value in configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_broken_conf_truncated.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Missing argument to keyword 'param' - it should be 'param("...")' on line 1 in %s/broken_configuration/config/config_broken_conf_truncated.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/config_broken_conf_truncated.ini on line 1
Could not startup.
