--TEST--
Broken configuration - missing quote
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_quotes.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] Invalid value '_SERVER[PHP_SELF' for `var` on line 1.
