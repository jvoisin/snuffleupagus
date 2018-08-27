--TEST--
Sloppy comparison
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/sloppy_comparison.ini
--FILE--
<?php 
$qwe = array(rand(1,2), "qwe");
var_dump(in_array(0, $qwe));
var_dump(in_array(0, $qwe, 0));
?>
--EXPECT--
bool(false)
bool(false)
