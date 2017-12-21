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
strlen("test");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in %a/tests/disabled_function_local_var_const.php:4 has been disabled.
