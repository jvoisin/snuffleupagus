--TEST--
Disable functions - allow
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_param_allow.ini
--FILE--
<?php 
system("echo win");
system("id");
?>
--EXPECTF--
win

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'system' in %a/disabled_functions_param_allow.php on line 3