--TEST--
Eval blacklist - nested eval
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/eval_backlist.ini
--FILE--
<?php 
$a = strlen("1337 1337 1337");
echo "Outside of eval: $a\n";
eval(
	"echo 'Inception lvl 1...\n';
   eval(
     'echo \"Inception lvl 2...\n\";
      eval(
				 \"echo \'Inception lvl 3...\n\';
          strlen(\'Limbo!\');
       \");
   ');
");
echo "After eval: $a\n";
?>
--EXPECTF--
Outside of eval: 14
Inception lvl 1...
Inception lvl 2...
Inception lvl 3...
[snuffleupagus][0.0.0.0][eval][drop] A call to strlen was tried in eval, droping it.
