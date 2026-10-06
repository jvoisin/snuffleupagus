--TEST--
Broken configuration filename without absolute path
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_filename.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration line: 'sp.disabled_functions': '.filename' must be an absolute path or a phar archive on line 1 in %s/broken_configuration/config/broken_conf_invalid_filename.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_invalid_filename.ini on line 1
Could not startup.
