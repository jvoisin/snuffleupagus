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
var_dump(array_search(0, $qwe));
var_dump(array_search(0, $qwe, 0));
var_dump(array_search(0, $qwe, 1));
var_dump(array_keys($qwe, 0));
var_dump(array_keys($qwe, 0, 0));
var_dump(array_keys($qwe, 0, 1));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
array(0) {
}
array(0) {
}
array(0) {
}
