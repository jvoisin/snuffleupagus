--TEST--
Borken configuration - encrypted cookie with no name
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies_noname.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] You must specify a cookie name/regexp on line 2 in Unknown on line 0

Fatal error: [snuffleupagus][config] You must specify a cookie name/regexp on line 2 in Unknown on line 0