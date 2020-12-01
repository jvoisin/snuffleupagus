--TEST--
Disable functions check on `ret`.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_ret_allow.ini
--FILE--
<?php 
echo strpos("pouet", "p");
echo stripos("pouet", "p");
?>
--EXPECT--
00