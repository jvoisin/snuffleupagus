--TEST--
Bad config, invalid samesite type.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_cookie_samesite.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] You must specify a valid value to samesite on line 1.
