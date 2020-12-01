--TEST--
Eval whitelist
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_whitelist_blacklist.ini
--FILE--
<?php 
function my_fun($p) {
	return "my_fun: $p";
}

$a = my_fun("1337 1337 1337");
echo "Outside of eval: $a\n";
eval('$a = my_fun("1234");');
echo "After allowed eval: $a\n";
eval('$a = cos(1234);');
echo "After eval: $a\n";
?>
--EXPECTF--
Outside of eval: my_fun: 1337 1337 1337
After allowed eval: my_fun: 1234

Fatal error: [snuffleupagus][0.0.0.0][Eval_whitelist][drop] The function 'cos' isn't in the eval whitelist, dropping its call. in %a/eval_backlist_whitelist.php(10) : eval()'d code on line 1