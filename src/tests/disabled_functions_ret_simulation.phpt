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
[snuffleupagus][0.0.0.0][disabled_function][simulation] The return to the function 'strpos' in %a/disabled_functions_ret_simulation.php:2 has been disabled, because the function returned '0', which matched a rule.
0
[snuffleupagus][0.0.0.0][disabled_function][simulation] The return to the function 'stripos' in %a/disabled_functions_ret_simulation.php:3 has been disabled, because the function returned '0', which matched the rule '1'.
0
[snuffleupagus][0.0.0.0][disabled_function][drop] The return to the function 'strcmp' in %a/disabled_functions_ret_simulation.php:4 has been disabled, because the function returned '0', which matched a rule.
