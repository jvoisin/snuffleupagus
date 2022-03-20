--TEST--
INI protection .min() + .policy_silent_fail()
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/config/sp-policy-silent-fail.ini
--FILE--
<?php 
var_dump(ini_set("max_execution_time", "29") === false);
var_dump(ini_get("max_execution_time"));
?>
--EXPECTF-- 
bool(true)
string(1) "0"