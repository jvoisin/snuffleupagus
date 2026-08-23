--TEST--
Config scanner - condition comparison operators (>=, <=, >) honour the boundary
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_condition_operators.ini
--FILE--
<?php
strtolower("a"); /* `5 > 5` is false: not disabled, runs fine */
strtoupper("b"); /* `5 >= 5 && 5 <= 5` is true: disabled */
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strtoupper' in %a/config_condition_operators.php on line 3
