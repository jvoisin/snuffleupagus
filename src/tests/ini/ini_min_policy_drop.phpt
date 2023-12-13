--TEST--
INI protection .min() + .policy_drop()
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/config/sp-policy-drop.ini
--FILE--
<?php 
var_dump(ini_set("max_execution_time", "29") === false);
var_dump(ini_get("max_execution_time"));
?>
--EXPECTF-- 
Fatal error: [snuffleupagus][0.0.0.0][ini_protection][drop] INI value 29 for `max_execution_time` out of range in %a/ini_min_policy_drop.php on line 2%A
