--TEST--
Eval whitelist/blacklist, on builtin functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_whitelist_blacklist.ini
--FILE--
<?php 
function my_fun($p) {
	return "my_fun: $p";
}

$a = tan(1);
echo "Outside of eval: $a\n";
eval('$a = tan(1);');
echo "After allowed eval: $a\n";
eval('$a = cos(1234);');
echo "After eval: $a\n";
?>
--EXPECTF--
Outside of eval: 1.5574077246549
After allowed eval: 1.5574077246549
[snuffleupagus][0.0.0.0][Eval_whitelist][drop] The function 'cos' isn't in the eval whitelist, dropping its call.
