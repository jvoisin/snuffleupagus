--TEST--
Disable functions - eval (simulation)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_eval_simulation.ini
--FILE--
<?php 
$var = 123456789;
eval('$var = 1337 + 1337;');
print("Variable: $var\n");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'eval' in %a/tests/disabled_functions_eval_simulation.php(%d) : eval()'d code:%d.
Variable: 2674
