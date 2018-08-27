--TEST--
Sloppy comparison
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/sloppy_comparison.ini
--FILE--
<?php 
var_dump(in_array(0, ["qwe", "asd"]));
var_dump(in_array(0, ["qwe", "asd"], 0));
?>
--EXPECT--
bool(false)
bool(false)
