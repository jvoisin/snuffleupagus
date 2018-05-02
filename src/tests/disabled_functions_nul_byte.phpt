--TEST--
Disable functions with nul byte
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_nul_byte.ini
--FILE--
<?php 
system("\0id");
system("id");

?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'system' in %a/tests/disabled_functions_nul_byte.php:2, because its argument '$command' content (0id) matched a rule.
