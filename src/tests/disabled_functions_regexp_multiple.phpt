--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_regexp.ini
--FILE--
<?php 
echo strlen("id") . "\n";
echo strcmp("1", "2") . "\n";
print("After") . "\n";
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][simulation] The call to the function 'strlen' in %a/disabled_functions_regexp_multiple.php:2 has been disabled.
2
[snuffleupagus][0.0.0.0][disabled_function][simulation] The call to the function 'strcmp' in %a/disabled_functions_regexp_multiple.php:3 has been disabled.
-1
After
