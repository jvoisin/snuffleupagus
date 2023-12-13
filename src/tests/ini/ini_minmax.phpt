--TEST--
INI protection .min()/.max()
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/config/sp.ini
--FILE--
<?php 
var_dump(ini_set("max_execution_time", "30") === false);
var_dump(ini_get("max_execution_time"));

var_dump(ini_set("max_execution_time", "300") === false);
var_dump(ini_get("max_execution_time"));

var_dump(ini_set("max_execution_time", "29") === false);
var_dump(ini_get("max_execution_time"));

var_dump(ini_set("max_execution_time", "301") === false);
var_dump(ini_get("max_execution_time"));

?>
--EXPECTF-- 
bool(false)
string(2) "30"
bool(false)
string(3) "300"

Warning: [snuffleupagus][0.0.0.0][ini_protection][log] INI value 29 for `max_execution_time` out of range in %a/ini_minmax.php on line 8
bool(true)
string(3) "300"

Warning: [snuffleupagus][0.0.0.0][ini_protection][log] INI value 301 for `max_execution_time` out of range in %a/ini_minmax.php on line 11
bool(true)
string(3) "300"%A
