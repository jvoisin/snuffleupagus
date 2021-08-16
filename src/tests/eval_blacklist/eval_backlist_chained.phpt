--TEST--
Eval blacklist - with several calls in an eval.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
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
Fatal error: [snuffleupagus][0.0.0.0][eval][drop] A call to 'strtoupper' was tried in eval. dropping it. in %s/eval_backlist_chained.php(%d) : eval()'d code on line %d
