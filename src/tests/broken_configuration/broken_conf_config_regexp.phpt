--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_config_regexp.ini
--FILE--
--EXPECTF--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Failed to compile '*.': %a. in Unknown on line 0
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Invalid regexp '*.' for '.filename_r()' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Failed to compile '*.': %a. in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid regexp '*.' for '.filename_r()' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
