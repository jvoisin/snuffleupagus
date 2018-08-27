--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_local_var_14.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] Invalid var name: $i+valid var name . in Unknown on line 0
PHP Fatal error:  [snuffleupagus][config] Invalid value '$i+valid var name ' for `var` on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid var name: $i+valid var name . in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid value '$i+valid var name ' for `var` on line 1 in Unknown on line 0