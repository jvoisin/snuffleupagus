--TEST--
Borken configuration - encrypted cookie with no name
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies_noname.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] You must specify a cookie name to encrypt on line 2.
