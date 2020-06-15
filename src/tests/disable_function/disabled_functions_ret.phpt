--TEST--
Disable functions check on `ret`.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret.ini
--FILE--
<?php 
echo strpos("pouet", "p");
echo stripos("pouet", "p");
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on return of the function 'strpos', because the function returned '0', which matched a rule in %a/disabled_functions_ret.php on line 2