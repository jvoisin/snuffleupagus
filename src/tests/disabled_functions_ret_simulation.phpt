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
[snuffleupagus][0.0.0.0][disabled_function][notice] The execution has been aborted in %a/disabled_functions_ret_simulation.php:2, because the return value (0) of the function 'strpos' matched a rule.
0
[snuffleupagus][0.0.0.0][disabled_function][notice] The execution has been aborted in %a/disabled_functions_ret_simulation.php:3, because the function 'stripos' returned '0', which matched the rule '1'.
0
[snuffleupagus][0.0.0.0][disabled_function][drop] The execution has been aborted in %a/disabled_functions_ret_simulation.php:4, because the return value (0) of the function 'strcmp' matched a rule.
