--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php
namespace qwe {
  const QWE = Array('123'=>'asdfgh');
}
namespace asd {
  const asd = 'qwe';
  echo "Valeur: " . \qwe\QWE[123]. "\n";
  strtoupper("qwe");
}
?>
--EXPECTF--
Valeur: asdfgh

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strtoupper' in %a/disabled_function_local_var_9.php on line 8
