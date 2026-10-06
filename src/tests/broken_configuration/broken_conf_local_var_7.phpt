--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_local_var_7.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid `->` position. in %s/broken_configuration/config/broken_conf_local_var_7.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid value 'asd-->' for `var` on line 1 in %s/broken_configuration/config/broken_conf_local_var_7.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_local_var_7.ini on line 1
Could not startup.
