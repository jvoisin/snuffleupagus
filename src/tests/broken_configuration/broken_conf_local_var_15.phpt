--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_local_var_15.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid var name: $i$$!@#. in %s/broken_configuration/config/broken_conf_local_var_15.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid value '$i$$!@#->qwe' for `var` on line 1 in %s/broken_configuration/config/broken_conf_local_var_15.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_local_var_15.ini on line 1
Could not startup.
