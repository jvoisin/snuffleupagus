--TEST--
Eval blacklist - with a list of functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_backlist_list.ini
--FILE--
<?php 
$a = strlen("1337 1337 1337");
echo "Outside of eval: $a\n";
eval('$a = strlen("1234");');
echo "After eval: $a\n";
?>
--EXPECTF--
Outside of eval: 14
[snuffleupagus][0.0.0.0][eval][drop] A call to strlen was tried in eval, in %a/tests/eval_backlist_list.php:1, dropping it.
