--TEST--
Sloppy comparison
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
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
