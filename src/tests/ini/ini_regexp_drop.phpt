--TEST--
INI protection .min() + .drop()
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/config/sp.ini
--FILE--
<?php 
var_dump(ini_set("user_agent", "Foo") === false);
var_dump(ini_get("user_agent"));
?>
--EXPECTF-- 
Fatal error: [snuffleupagus][0.0.0.0][ini_protection][drop] INI value does not match regex in %a/ini_regexp_drop.php on line 2%A%A%A%A
