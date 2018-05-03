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
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on return of the function 'system' in %a/disabled_functions_name_r.php:3, because the function returned '1337', which matched a rule.
