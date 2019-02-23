--TEST--
Eval blacklist - with several calls in an eval.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_backlist.ini
--FILE--
<?php 
eval('
	cos(1);
	strtoupper(2);
	sin(3);
')
?>
--EXPECTF--
Fatal error: [snuffleupagus][eval] A call to strtoupper was tried in eval, in %s/eval_backlist_chained.php:%d, dropping it. in %s/eval_backlist_chained.php(%d) : eval()'d code on line %d
