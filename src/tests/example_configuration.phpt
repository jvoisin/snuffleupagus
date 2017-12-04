--TEST--
Shipped configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../config/examples.ini
--FILE--
<?php 
echo 0;
?>
--EXPECTF--
0
