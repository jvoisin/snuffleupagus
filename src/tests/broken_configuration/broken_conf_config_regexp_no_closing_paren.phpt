--TEST--
Broken configuration - regexp without a closing parenthesis
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_config_regexp_no_closing_paren.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Missing argument to keyword 'filename_r' - it should be 'filename_r("...")' on line 1 in %s/broken_configuration/config/broken_config_regexp_no_closing_paren.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_config_regexp_no_closing_paren.ini on line 1
Could not startup.
