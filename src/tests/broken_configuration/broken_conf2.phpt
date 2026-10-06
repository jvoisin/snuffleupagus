--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf2.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Unexpected keyword 'wrong' in %s/tests/broken_configuration/config/broken_conf2.ini:1 in %s/broken_configuration/config/broken_conf2.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf2.ini on line 1
Could not startup.
