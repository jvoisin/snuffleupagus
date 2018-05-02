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
[snuffleupagus][0.0.0.0][disabled_function][drop] The return to the function 'cos' %a/disabled_functions_ret_type_double.php:%d has been disabled, because the function returned '0.877583', which matched the rule 'Return value is a double'.
