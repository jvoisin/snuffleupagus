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
[snuffleupagus][0.0.0.0][disabled_function][drop] The execution has been aborted in %a/disabled_functions_name_r.php:3, because the return value (1337) of the function 'system' matched a rule.
