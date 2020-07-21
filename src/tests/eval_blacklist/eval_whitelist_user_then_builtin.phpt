--TEST--
Eval whitelist - builtin function
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_whitelist.ini
--FILE--
<?php 

function my_fun() {
	return sin(10);
}

$a = my_fun(1);
echo "Outside of eval: $a\n";
eval('$a = my_fun(5);');
echo "After allowed eval: $a\n";
eval('$a = my_fun(4);');
echo "After eval: $a\n";
?>
--EXPECTF--
Outside of eval: -0.54402111088937

Fatal error: [snuffleupagus][0.0.0.0][Eval_whitelist][drop] The function 'sin' isn't in the eval whitelist, dropping its call. in %a/eval_whitelist_user_then_builtin.php on line 4