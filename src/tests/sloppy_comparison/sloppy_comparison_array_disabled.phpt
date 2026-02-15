--TEST--
Sloppy comparison in_array disabled
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip Not applicable for PHP 8+"; ?>
--INI--
sp.allow_broken_configuration=On
--FILE--
<?php 
$qwe = array(rand(1,2), "qwe");
var_dump(in_array(0, $qwe));
var_dump(in_array(0, $qwe, 0));
var_dump(in_array(0, $qwe, 1));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
