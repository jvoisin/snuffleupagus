--TEST--
Broken configuration - encrypted cookie with no name
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies_noname.ini
error_log=/dev/null
--FILE--
--EXPECT--
Fatal error: [snuffleupagus][0.0.0.0][config][log] You must specify a cookie name/regexp on line 2 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
