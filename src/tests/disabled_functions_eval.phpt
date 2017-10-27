--TEST--
Disable functions - eval
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_eval.ini
--FILE--
<?php 
$var = 123456789;
eval('$var = 1337 + 1337;');
print("Variable: $var\n");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'eval' in %a/tests/disabled_functions_eval.php(3) : eval()'d code:1 has been disabled.
