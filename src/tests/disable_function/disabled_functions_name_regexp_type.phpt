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

Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'strcmp', because its argument 'str1' content (?) matched a rule in %a/disabled_functions_name_regexp_type.php on line 4