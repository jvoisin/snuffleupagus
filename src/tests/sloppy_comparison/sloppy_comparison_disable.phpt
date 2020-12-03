--TEST--
Sloppy comparison
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
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
