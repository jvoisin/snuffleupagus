--TEST--
Sloppy comparison
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/sloppy_comparison.ini
--FILE--
<?php 
$qwe = "abc";
if ($qwe == 0) {
  echo "failed";
}
$qwe = "0e123";
if ("0e432" == $qwe) {
  echo "failed";
}
$qwe = [];
$test = false;
if ($test != $qwe) {
  echo "lol";
}
eval("
\$asd = 'qwe';
if (\$asd == 0) {
  echo 'oops';
}
");
?>
--EXPECT--
lol
