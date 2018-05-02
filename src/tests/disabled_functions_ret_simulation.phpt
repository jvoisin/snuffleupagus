--TEST--
Disable functions check on `ret` simulation
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_ret_simulation.ini
--FILE--
<?php 
echo strpos("pouet", "p") . "\n";
echo stripos("pouet", "p") . "\n";
strcmp("p", "p") . "\n";
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on return of the function 'strpos' in %a/disabled_functions_ret_simulation.php:2, because the function returned '0', which matched a rule.
0
[snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on return of the function 'stripos' in %a/disabled_functions_ret_simulation.php:3, because the function returned '0', which matched the rule '1'.
0
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on return of the function 'strcmp' in %a/disabled_functions_ret_simulation.php:4, because the function returned '0', which matched a rule.
