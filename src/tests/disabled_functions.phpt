--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions.ini
--FILE--
<?php 
system("id");
printf("printf in simulation mode\n");
print("print in disabled mode\n");
var_dump("this is a super test");
echo strpos("pouet", "o");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'system' in %a/tests/disabled_functions.php:2.

