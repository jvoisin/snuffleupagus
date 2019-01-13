--TEST--
Eval blacklist - with several calls in an eval.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_backlist.ini
--FILE--
<?php 
eval('
	call_user_func("strlen", 2);
')
?>
--EXPECTF--
Fatal error: [snuffleupagus][eval] A call to strlen was tried in eval, in %s/eval_backlist_call_user_func.php:%d, dropping it. in %s/eval_backlist_call_user_func.php(%d) : eval()'d code on line %d
