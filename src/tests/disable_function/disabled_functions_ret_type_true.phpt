--TEST--
Disable functions check on `ret` by type matching (true).
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type_true.ini
--FILE--
<?php 
var_dump(is_numeric("pouet")) . "\n";
echo "1337\n";
echo is_numeric("1234") . "\n";
?>
--EXPECTF--
bool(false)
1337

Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on return of the function 'is_numeric', because the function returned 'TRUE', which matched the rule 'Return value is a true' in %a/disabled_functions_ret_type_true.php on line 4