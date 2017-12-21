--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_local_var_15.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] Invalid var name: $i$$!@#.
[snuffleupagus][0.0.0.0][config][error] Invalid value '$i$$!@#->qwe' for `var` on line 1.
