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
[snuffleupagus][0.0.0.0][disabled_function][drop] The execution has been aborted in %a/disabled_functions_ret_val.php:3, because the return value (fufufu) of the function 'str_repeat' matched a rule.
