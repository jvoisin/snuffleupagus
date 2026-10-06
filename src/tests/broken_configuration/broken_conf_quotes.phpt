--TEST--
Broken configuration - missing quote
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_quotes.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] You forgot to close a bracket. in %s/broken_configuration/config/broken_conf_quotes.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid value '_SERVER[PHP_SELF' for `var` on line 1 in %s/broken_configuration/config/broken_conf_quotes.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_quotes.ini on line 1
Could not startup.
