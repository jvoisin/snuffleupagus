--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_name_regexp_type.ini
--FILE--
<?php 
echo strcmp("pouet", "pouet") . "\n";
echo strcmp(1, 2) . "\n";
echo strcmp([1], "pouet") . "\n";
?>
--EXPECTF--
0
-1
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strcmp' in %a/disabled_functions_name_regexp_type.php:4, because its argument 'str1' content (?) matched a rule.
