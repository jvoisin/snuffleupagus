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
[snuffleupagus][0.0.0.0][disabled_function][drop] The return to the function 'is_numeric' in %a/disabled_functions_ret_type_true.php:%d has been disabled, because the function returned 'TRUE', which matched the rule 'Return value is a true'.
