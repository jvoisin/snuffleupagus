--TEST--
Disable functions check on `ret`.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret.ini
--FILE--
<?php 
echo strpos("pouet", "p");
echo stripos("pouet", "p");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The return to the function 'strpos' in %a/disabled_functions_ret.php:2 has been disabled, because the function returned '0', which matched a rule.
