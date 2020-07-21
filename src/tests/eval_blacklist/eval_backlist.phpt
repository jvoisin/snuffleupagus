--TEST--
Eval blacklist
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_backlist.ini
--FILE--
<?php 
$a = strtoupper("1337 1337 1337");
echo "Outside of eval: $a\n";
eval('$a = strtoupper("1234");');
echo "After eval: $a\n";
?>
--EXPECTF--
Outside of eval: 1337 1337 1337

Fatal error: [snuffleupagus][0.0.0.0][eval][drop] A call to strtoupper was tried in eval, in %a/eval_backlist.php:1, dropping it. in %a/eval_backlist.php(4) : eval()'d code on line 1
