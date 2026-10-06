--TEST--
Broken configuration - encrypted cookie with name and regexp
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_cookie_name_and_regexp.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] name and name_r are mutually exclusive in %s/tests/broken_configuration/config/broken_conf_cookie_name_and_regexp.ini:2 in %s/broken_configuration/config/broken_conf_cookie_name_and_regexp.ini on line 2

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_cookie_name_and_regexp.ini on line 2
Could not startup.
