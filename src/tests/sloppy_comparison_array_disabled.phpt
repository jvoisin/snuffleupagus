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
var_dump(array_search(0, $qwe));
var_dump(array_search(0, $qwe, 0));
var_dump(array_search(0, $qwe, 1));
var_dump(array_keys($qwe, 0));
var_dump(array_keys($qwe, 0, 0));
var_dump(array_keys($qwe, 0, 1));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
int(1)
int(1)
bool(false)
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
array(0) {
}
