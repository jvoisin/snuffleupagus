--TEST--
Bad boolean value in configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_expecting_bool.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Missing parenthesis for keyword 'enable' - it should be 'enable()' in %s/tests/broken_configuration/config/broken_conf_expecting_bool.ini:5 in %s/broken_configuration/config/broken_conf_expecting_bool.ini on line 5

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_expecting_bool.ini on line 5
Could not startup.
