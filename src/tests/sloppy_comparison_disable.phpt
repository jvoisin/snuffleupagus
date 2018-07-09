--TEST--
Sloppy comparison
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
--FILE--
<?php 
$qwe = "abc";
if ($qwe == 0) {
  echo "OK";
}
?>
--EXPECT--
OK
