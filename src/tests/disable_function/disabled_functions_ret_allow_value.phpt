--TEST--
Disable functions check on `ret` allowed
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_ret_allow_value.ini
--FILE--
<?php 
echo strpos("pouet", "p");
?>
--EXPECT--
0
