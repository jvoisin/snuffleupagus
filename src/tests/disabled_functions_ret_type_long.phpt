--TEST--
Disable functions check on `ret` by type matching (long).
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type_long.ini
--FILE--
<?php 
echo strlen("pouet") . "\n";
?>
--EXPECTF--
Fatal error: [snuffleupagus][disabled_function] Aborted execution on return of the function 'strlen', because the function returned '5', which matched the rule 'Return value is a long' in %a/disabled_functions_ret_type_long.php on line 2