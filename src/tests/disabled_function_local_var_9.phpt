--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
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
  strlen("qwe");
}
?>
--EXPECTF--
Valeur: asdfgh
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in %a/tests/disabled_function_local_var_9.php:%d has been disabled.
