--TEST--
INI protection .regexp()
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/config/sp.ini
--FILE--
<?php 
ini_set("highlight.comment", "#000aBc");
var_dump(ini_get("highlight.comment"));

ini_set("highlight.comment", "xxx");
var_dump(ini_get("highlight.comment"));
?>
--EXPECTF-- 
string(7) "#000aBc"

Warning: [snuffleupagus][0.0.0.0][ini_protection][log] INI value `xxx` for `highlight.comment` does not match regex in %a/ini_regexp.php on line 5
string(7) "#000aBc"%A
