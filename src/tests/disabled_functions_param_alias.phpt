--TEST--
Disable functions - alias
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_param_alias.ini
--FILE--
<?php 
system("id");
shell_exec("id");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'system' in %a/tests/disabled_functions_param_alias.php:2 has been disabled, because of the the rule '1'.
[snuffleupagus][0.0.0.0][disabled_function][notice] The call to the function 'shell_exec' in %a/tests/disabled_functions_param_alias.php:3 has been disabled, because of the the rule '2'.
