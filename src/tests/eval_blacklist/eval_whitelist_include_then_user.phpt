--TEST--
Eval whitelist - builtin function
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_whitelist.ini
--FILE--
<?php 
$b = 1337;
$dir = __DIR__;

// Just in case
@unlink($dir . '/test.bla');

file_put_contents($dir . '/test.bla', '<?php $b = sin(1) ?>');

$a = cos(1);
echo "Outside of eval: $a\n";
eval('$a = cos(5);');
echo "After allowed eval: $a\n";
eval("include_once('$dir' . '/test.bla');");
echo "After eval: $b\n";
?>
--EXPECTF--
Outside of eval: 0.54030230586814
After allowed eval: 0.28366218546323

Fatal error: [snuffleupagus][0.0.0.0][Eval_whitelist][drop] The function 'sin' isn't in the eval whitelist, dropping its call. in %a/test.bla on line 1
