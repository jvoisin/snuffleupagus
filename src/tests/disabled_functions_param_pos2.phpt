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
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in %a/disabled_functions_param_pos2.php:3 has been disabled, because its argument 'str' content (id) matched the rule 'strlen array'.
