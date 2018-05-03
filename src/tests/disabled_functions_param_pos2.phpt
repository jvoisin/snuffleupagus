--TEST--
Disable functions - match on argument's position, not the first time
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_pos.ini
--FILE--
<?php 
strlen("od");
strlen("id");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strlen' in %a/disabled_functions_param_pos2.php:3, because its argument 'str' content (id) matched the rule 'strlen array'.
