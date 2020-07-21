--TEST--
Broken configuration - encrypted cookie without encryption key
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_cookie_encryption_without_encryption_key.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] You're trying to use the cookie encryption featureon line 2 without having set the `.encryption_key` option in`sp.global`: please set it first in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] You're trying to use the cookie encryption featureon line 2 without having set the `.encryption_key` option in`sp.global`: please set it first in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
