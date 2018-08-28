--TEST--
Sloppy comparison
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
--FILE--
<?php 
$qwe = "abc";
if ($qwe == 0) {
  echo "OK\n";
}
$qwe = array(rand(1,2), "qwe");
var_dump(in_array(0, $qwe));
?>
--EXPECT--
OK
bool(true)
