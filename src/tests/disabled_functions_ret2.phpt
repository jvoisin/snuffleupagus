--TEST--
Disable functions check on `ret`.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret.ini
--FILE--
<?php 
echo stripos("pouet", "p");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'stripos' in %a/disabled_functions_ret2.php:2, because the function returned '0', which matched a rule.
