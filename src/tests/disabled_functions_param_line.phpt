--TEST--
Disable functions - match on argument's position
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_pos.ini
--FILE--
<?php 
system("echo 1337");
system("id");
?>
--EXPECTF--
1337
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'system' in %a/disabled_functions_param_line.php:3 has been disabled, because its argument 'command' content (id) matched a rule.
