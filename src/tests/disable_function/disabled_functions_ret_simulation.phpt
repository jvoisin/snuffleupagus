--TEST--
Disable functions check on `ret` simulation
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_ret_simulation.ini
--FILE--
<?php 
echo strpos("pouet", "p") . "\n";
echo stripos("pouet", "p") . "\n";
strcmp("p", "p") . "\n";
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on return of the function 'strpos', because the function returned '0', which matched a rule in %a/disabled_functions_ret_simulation.php on line 2
0

Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on return of the function 'stripos', because the function returned '0', which matched the rule '1' in %a/disabled_functions_ret_simulation.php on line 3
0

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on return of the function 'strcmp', because the function returned '0', which matched a rule in %a/disabled_functions_ret_simulation.php on line 4