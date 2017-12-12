--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_name_type.ini
--FILE--
<?php 
echo strcmp("pouet", "pouet") . "\n";
echo strcmp([1,23], "pouet") . "\n";
?>
--EXPECTF--
0
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strcmp' in %a/disabled_functions_name_type.php:%d has been disabled, because its argument 'str1' content (ARRAY) matched a rule.
