--TEST--
Broken config, session encryption
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_session_encryption.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Missing parenthesis for keyword 'encrypt' - it should be 'encrypt()' in %s/tests/broken_configuration/config/broken_conf_session_encryption.ini:1 in %s/broken_configuration/config/broken_conf_session_encryption.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_session_encryption.ini on line 1
Could not startup.
