--TEST--
Eval blacklist - nested eval, with a twist
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_backlist.ini
--FILE--
<?php 
$a = strtoupper("1337 1337 1337");
echo "Outside of eval: $a\n";
eval(
	"echo 'Inception lvl 1...\n';
   eval(
     'echo \"Inception lvl 2...\n\";
      eval(
				 \"echo \'Inception lvl 3...\n\';
       \");
			 strtoupper(\'Limbo!\');
   ');
");
echo "After eval: $a\n";
?>
--EXPECTF--
Outside of eval: 1337 1337 1337
Inception lvl 1...
Inception lvl 2...
Inception lvl 3...

Fatal error: [snuffleupagus][0.0.0.0][eval][drop] A call to strtoupper was tried in eval, in %a/nested_eval_blacklist2.php(5) : eval()'d code:7, dropping it. in %a/nested_eval_blacklist2.php(5) : eval()'d code(4) : eval()'d code on line 7
