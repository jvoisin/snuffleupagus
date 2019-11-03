--TEST--
Disable functions check on `ret` by type matching (double).
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type_double.ini
--FILE--
<?php 
echo cos(0.5) . "\n";
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on return of the function 'cos', because the function returned '0.877583', which matched the rule 'Return value is a double' in %a/disabled_functions_ret_type_double.php on line 2