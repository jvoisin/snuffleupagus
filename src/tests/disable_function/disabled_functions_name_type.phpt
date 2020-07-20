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

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strcmp', because its argument '$str1' content (ARRAY) matched a rule in %a/disabled_functions_name_type.php on line 3