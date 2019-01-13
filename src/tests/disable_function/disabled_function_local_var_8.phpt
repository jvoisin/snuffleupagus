--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php
namespace qwe {
  const ASD = 'qwerty';
}
namespace asd {
  const asd = 'qwe';
  echo "Valeur: " . \qwe\ASD . "\n";
  strlen("qwe");
}
?>
--EXPECTF--
Valeur: qwerty

Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'strlen' in %a/disabled_function_local_var_8.php on line 8