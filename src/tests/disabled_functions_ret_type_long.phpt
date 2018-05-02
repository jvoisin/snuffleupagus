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
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on return of the function 'strlen' in %a/disabled_functions_ret_type_long.php:%d, because the function returned '5', which matched the rule 'Return value is a long'.
