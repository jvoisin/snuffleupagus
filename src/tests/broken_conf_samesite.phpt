--TEST--
Bad config, invalid samesite type.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_cookie_samesite.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] nop is an invalid value to samesite (expected Lax or Strict) on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] nop is an invalid value to samesite (expected Lax or Strict) on line 1 in Unknown on line 0