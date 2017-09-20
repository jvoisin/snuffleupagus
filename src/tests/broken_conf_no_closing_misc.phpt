--TEST--
Configuration line without closing parenthese, misc
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_no_closing_misc.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] Missing closing ) in line 123.
[snuffleupagus][0.0.0.0][error][error] .mask_ipv4() is expecting a valid integer.
