--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_local_var_15.ini
--FILE--
--EXPECT--

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid var name: $i$$!@#. in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid value '$i$$!@#->qwe' for `var` on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
