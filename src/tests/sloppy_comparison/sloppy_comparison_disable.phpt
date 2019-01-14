--TEST--
Sloppy comparison
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.allow_broken_configuration=On
--FILE--
<?php 
$qwe = "abc";
if ($qwe == 0) {
  echo "OK";
}
?>
--EXPECT--
OK
