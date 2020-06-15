--TEST--
Disable functions - match on a constant
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var_const.ini
--FILE--
<?php
$a = 1337;
define("MY_CONST", $a);
strtoupper("test");
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'strtoupper' in %a/disabled_function_local_var_const.php on line 4