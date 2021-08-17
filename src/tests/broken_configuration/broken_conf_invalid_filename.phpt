--TEST--
Broken configuration filename without absolute path
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_filename.ini
--FILE--
--EXPECTF--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Invalid configuration line: 'sp.disabled_functions': '.filename' must be an absolute path or a phar archive on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration line: 'sp.disabled_functions': '.filename' must be an absolute path or a phar archive on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
