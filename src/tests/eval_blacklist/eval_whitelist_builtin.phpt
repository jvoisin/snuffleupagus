--TEST--
Eval whitelist - builtin function
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_whitelist.ini
--FILE--
<?php 
$a = cos(1);
echo "Outside of eval: $a\n";
eval('$a = cos(5);');
echo "After allowed eval: $a\n";
eval('$a = sin(4);');
echo "After eval: $a\n";
?>
--EXPECTF--
Outside of eval: 0.54030230586814
After allowed eval: 0.28366218546323

Fatal error: [snuffleupagus][0.0.0.0][Eval_whitelist][drop] The function 'sin' isn't in the eval whitelist, dropping its call. in %a/eval_whitelist_builtin.php(6) : eval()'d code on line 1