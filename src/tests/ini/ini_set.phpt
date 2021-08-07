--TEST--
INI protection .set()
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/config/sp.ini
--FILE--
<?php 
var_dump(ini_get("default_mimetype"));
?>
--EXPECTF-- 
string(10) "text/plain"