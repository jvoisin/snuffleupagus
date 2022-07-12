--TEST--
Disable functions - eval, on matching parameter
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_eval_param.ini
--FILE--
<?php 
$var = 123456789;
eval('$var = 1337 + 1337;');
print("Variable: $var\n");
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'eval', because its argument 'code' content ($var = 1337 + 1337;) matched a rule in %s/tests/disable_function/disabled_functions_eval_param.php(3) : eval()'d code on line 1
