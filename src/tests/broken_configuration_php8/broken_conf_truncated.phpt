--TEST--
Bad boolean value in configuration
--SKIPIF--
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_broken_conf_truncated.ini
--FILE--
--EXPECT--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Missing argument to keyword 'param' - it should be 'param("...")' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
