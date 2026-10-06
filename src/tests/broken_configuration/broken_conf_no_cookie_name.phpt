--TEST--
Broken configuration - encrypted cookie with no name
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies_noname.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] You must specify a cookie name/regexp in %s/tests/broken_configuration/config/config_encrypted_cookies_noname.ini:2 in %s/broken_configuration/config/config_encrypted_cookies_noname.ini on line 2

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/config_encrypted_cookies_noname.ini on line 2
Could not startup.
