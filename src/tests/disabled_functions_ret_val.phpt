--TEST--
Disable functions ret val
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_retval.ini
--FILE--
<?php 
echo str_repeat("fufu",1)."\n";
echo str_repeat("fufufu",1);
?>
--EXPECTF--
fufu
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'str_repeat' in %a/disabled_functions_ret_val.php:3, because the function returned 'fufufu', which matched a rule.
