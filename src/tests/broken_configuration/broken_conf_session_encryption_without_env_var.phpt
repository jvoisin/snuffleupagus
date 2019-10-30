--TEST--
Broken configuration - encrypted session without env var
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_session_encryption_without_env_var.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config] You're trying to use the session cookie encryption feature on line 2 without having set the `.cookie_env_var` option in`sp.global`: please set it first in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] You're trying to use the session cookie encryption feature on line 2 without having set the `.cookie_env_var` option in`sp.global`: please set it first in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Invalid configuration file in Unknown on line 0
Could not startup.
