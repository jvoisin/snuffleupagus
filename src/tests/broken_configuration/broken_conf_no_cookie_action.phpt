--TEST--
Bad config, invalid action.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_cookie_action.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] You must specify a at least one action to a cookie in %s/tests/broken_configuration/config/broken_conf_cookie_action.ini:1 in %s/broken_configuration/config/broken_conf_cookie_action.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_cookie_action.ini on line 1
Could not startup.
