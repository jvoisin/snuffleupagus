--TEST--
Eval blacklist - with a list of functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_backlist_list.ini
--FILE--
<?php 
$a = strtoupper("1337 1337 1337");
echo "Outside of eval: $a\n";
eval('$a = strtoupper("1234");');
echo "After eval: $a\n";
?>
--EXPECTF--
Outside of eval: 1337 1337 1337

Fatal error: [snuffleupagus][0.0.0.0][eval][drop] A call to 'strtoupper' was tried in eval. dropping it. in %a/eval_backlist_list.php(4) : eval()'d code on line 1
