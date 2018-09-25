--TEST--
Sloppy comparison array_search with keys
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/sloppy_comparison.ini
--FILE--
<?php 
$array = array(0 => 'blue', 1 => 'red', 2 => 'green', 3 => 'red');
var_dump(array_search('green', $array));
?>
--EXPECT--
int(2)
