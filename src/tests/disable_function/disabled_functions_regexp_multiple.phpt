--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_regexp.ini
--FILE--
<?php 
echo strtoupper("id") . "\n";
echo strcmp("1", "2") . "\n";
print("After") . "\n";
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'strtoupper' in %a/disabled_functions_regexp_multiple.php on line 2
ID

Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'strcmp' in %a/disabled_functions_regexp_multiple.php on line 3
-1
After
