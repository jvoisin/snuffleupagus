--TEST--
Disable functions - filename regexp
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_filename_r.ini
--FILE--
<?php 
system("echo 42");
shell_exec("echo 43");
?>
--EXPECTF--
42

Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'shell_exec' in %a/disabled_functions_filename_r.php on line 3