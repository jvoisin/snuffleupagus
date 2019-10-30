--TEST--
Disabled native functions, called indirectly
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions.ini
--FILE--
<?php 
array_map('system', [1,2,3,4]);
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'system' in %a/disabled_native_functions_indirect.php on line 2