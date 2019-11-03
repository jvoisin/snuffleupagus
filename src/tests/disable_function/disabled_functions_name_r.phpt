--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_name_r.ini
--FILE--
<?php 
system("echo 42");
system("echo 1337");
?>
--EXPECTF--
42
1337

Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on return of the function 'system', because the function returned '1337', which matched a rule in %a/disabled_functions_name_r.php on line 3