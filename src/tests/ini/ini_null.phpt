--TEST--
INI protection .allow_null()
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/config/sp.ini
--FILE--
<?php 
var_dump(ini_set("sendmail_from", "foo@example.com") === false);
var_dump(ini_get("sendmail_from"));

var_dump(ini_set("sendmail_from", NULL) === false);
var_dump(ini_get("sendmail_from"));

var_dump(ini_set("unserialize_callback_func", NULL) === false);
var_dump(ini_get("unserialize_callback_func"));
?>
--EXPECTF-- 
bool(false)
string(15) "foo@example.com"
bool(false)
string(0) ""

Warning: [snuffleupagus][0.0.0.0][ini_protection][log] new INI value must not be NULL or empty in %a/ini_null.php on line 8
bool(true)
string(3) "def"