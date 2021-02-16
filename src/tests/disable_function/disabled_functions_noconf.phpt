--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/empty.ini
--FILE--
<?php 
echo strpos("pouet", "o");
?>
--EXPECT--
1
