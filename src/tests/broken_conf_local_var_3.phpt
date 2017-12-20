--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_local_var_3.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] Invalid `->` position.
[snuffleupagus][0.0.0.0][config][error] Invalid value '$qwe->::' for `var` on line 1.
