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
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'system' in%a/tests/disabled_native_functions_indirect.php:2 has been disabled.
