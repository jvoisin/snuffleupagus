--TEST--
Shipped configuration (rips)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../config/rips.rules
--FILE--
<?php 
system("echo 0");
?>
--EXPECTF--
0
