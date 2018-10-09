--TEST--
Broken configuration - encrypted session without encryption key
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_session_encryption_without_encryption_key.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] You're trying to use the session cookie encryption feature on line 2 without having set the `.secret_key` option in`sp.global`: please set it first in Unknown on line 0

Fatal error: [snuffleupagus][config] You're trying to use the session cookie encryption feature on line 2 without having set the `.secret_key` option in`sp.global`: please set it first in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.
