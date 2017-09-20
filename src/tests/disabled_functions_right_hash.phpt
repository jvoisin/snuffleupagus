--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_right_hash.ini
--FILE--
<?php 
system("echo $((1 + 1336))");
?>
--EXPECTF--
1337
