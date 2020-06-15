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
Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'system', because of the the rule '1' in %a/disabled_functions_param_alias.php on line 2