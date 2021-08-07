--TEST--
INI protection .min()/.max()
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/config/sp.ini
--FILE--
<?php 
var_dump(ini_set("log_errors_max_len", "200") === false);
var_dump(ini_get("log_errors_max_len"));

var_dump(ini_set("log_errors_max_len", "2000") === false);
var_dump(ini_get("log_errors_max_len"));

var_dump(ini_set("log_errors_max_len", "199") === false);
var_dump(ini_get("log_errors_max_len"));

var_dump(ini_set("log_errors_max_len", "2001") === false);
var_dump(ini_get("log_errors_max_len"));

?>
--EXPECTF-- 
bool(false)
string(3) "200"
bool(false)
string(4) "2000"

Warning: [snuffleupagus][0.0.0.0][ini_protection][log] INI value out of range in %a/ini_minmax.php on line 8
bool(true)
string(4) "2000"

Warning: [snuffleupagus][0.0.0.0][ini_protection][log] INI value out of range in %a/ini_minmax.php on line 11
bool(true)
string(4) "2000"