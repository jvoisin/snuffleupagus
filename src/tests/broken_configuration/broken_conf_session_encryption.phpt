--TEST--
Broken config, session encryption
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_session_encryption.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config] Trailing chars 'nvalid value :/);' at the end of '.encrypt(invalid value :/);' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Trailing chars 'nvalid value :/);' at the end of '.encrypt(invalid value :/);' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Invalid configuration file in Unknown on line 0
Could not startup.
