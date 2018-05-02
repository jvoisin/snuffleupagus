--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_ret_right_hash.ini
--FILE--
<?php 
system("echo $((1 + 1336))");
?>
--EXPECTF--
1337
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'system' in %a/tests/disabled_functions_ret_right_hash.php:2, because the function returned '1337', which matched a rule.
