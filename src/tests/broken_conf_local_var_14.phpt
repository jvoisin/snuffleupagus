--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_local_var_14.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] Invalid var name: $i+valid var name .
[snuffleupagus][0.0.0.0][config][error] Invalid value '$i+valid var name ' for `var` on line 1.
