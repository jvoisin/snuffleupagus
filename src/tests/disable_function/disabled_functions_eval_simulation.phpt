--TEST--
Disable functions - eval (simulation)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_eval_simulation.ini
--FILE--
<?php 
$var = 123456789;
eval('$var = 1337 + 1337;');
print("Variable: $var\n");
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'eval' in %a/disabled_functions_eval_simulation.php(3) : eval()'d code on line 1
Variable: 2674