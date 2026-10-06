--TEST--
Bad config, invalid samesite type.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_cookie_samesite.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] 'nop' is an invalid value to samesite (expected Lax or Strict) in %s/tests/broken_configuration/config/broken_conf_cookie_samesite.ini:1 in %s/broken_configuration/config/broken_conf_cookie_samesite.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_cookie_samesite.ini on line 1
Could not startup.
