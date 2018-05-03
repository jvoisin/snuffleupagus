--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_param_r.ini
--FILE--
<?php 
system("id");
system("echo win");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'system' in %a/tests/disabled_functions_param_r.php:2, because its argument 'command' content (id) matched a rule.
