--TEST--
Sloppy comparison array_search disabled
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--FILE--
<?php 
$qwe = array(rand(1,2), "qwe");
var_dump(array_search(0, $qwe));
var_dump(array_search(0, $qwe, 0));
var_dump(array_search(0, $qwe, 1));
?>
--EXPECT--
int(1)
int(1)
bool(false)
