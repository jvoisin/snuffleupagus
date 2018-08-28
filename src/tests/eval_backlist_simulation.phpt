--TEST--
Eval blacklist simulation
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_backlist_simulation.ini
--FILE--
<?php 
$a = strlen("1337 1337 1337");
echo "Outside of eval: $a\n";
eval('$a = strlen("1234");');
echo "After eval: $a\n";
?>
--EXPECTF--
Outside of eval: 14

Warning: [snuffleupagus][eval] A call to strlen was tried in eval, in %a/eval_backlist_simulation.php:1, logging it. in %a/eval_backlist_simulation.php(4) : eval()'d code on line 1
After eval: 4