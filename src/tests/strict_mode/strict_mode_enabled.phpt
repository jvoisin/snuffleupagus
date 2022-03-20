--TEST--
Strict mode enabled
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) print "skip snuffleupagus extension missing";
?>
--INI--
sp.configuration_file={PWD}/config/config_strict_mode_enabled.ini
--FILE--
<?php 
ini_get(23);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: ini_get()%s given in %s/tests/strict_mode/strict_mode_enabled.php:%d
Stack trace:
#0 %s/tests/strict_mode/strict_mode_enabled.php(2): ini_get(23)
#1 {main}
  thrown in %s/tests/strict_mode/strict_mode_enabled.php on line 2
