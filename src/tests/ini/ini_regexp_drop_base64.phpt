--TEST--
INI protection .min() + .drop(), log base64
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/config/sp.ini
--FILE--
<?php 
var_dump(ini_set("user_agent", "Foo\n\r") === false);
var_dump(ini_get("user_agent"));
?>
--EXPECTF-- 
Fatal error: [snuffleupagus][0.0.0.0][ini_protection][drop] INI value `Rm9vCg0=`(base64) for `user_agent` does not match regex in %a/ini_regexp_drop_base64.php on line 2%A%A%A%A
