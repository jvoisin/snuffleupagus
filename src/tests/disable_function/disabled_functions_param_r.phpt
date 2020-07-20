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
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'system', because its argument 'command' content (id) matched a rule in %a/disabled_functions_param_r.php on line 2