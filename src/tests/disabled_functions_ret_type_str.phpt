--TEST--
Disable functions check on `ret` by type matching (string).
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type_str.ini
--FILE--
<?php 
echo substr("pouet", 3) . "\n";
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'substr' in %a/disabled_functions_ret_type_str.php:%d, because the function returned 'et', which matched the rule 'Return value is a string'.
