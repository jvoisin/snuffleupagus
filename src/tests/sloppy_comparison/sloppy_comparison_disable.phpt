--TEST--
Sloppy comparison
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip Not applicable for PHP 8+"; ?>
--INI--
sp.allow_broken_configuration=On
--FILE--
<?php 
$qwe = "abc";
if ($qwe == 0) {
  echo "ONE";
}
$qwe = "0e123";
if ("0e432" == $qwe) {
  echo "TWO";
}
$qwe = [];
$test = false;
if ($test == $qwe) {
  echo "THREE";
}
eval("
\$asd = 'qwe';
if (\$asd == 0) {
  echo 'FOUR';
}
");
?>
--EXPECT--
ONETWOTHREEFOUR
