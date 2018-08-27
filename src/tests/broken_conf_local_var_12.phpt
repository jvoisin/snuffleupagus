--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_local_var_12.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] Empty value in `var` on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Empty value in `var` on line 1 in Unknown on line 0