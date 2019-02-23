--TEST--
Disable functions - match on argument's position, not the first time
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_pos.ini
--FILE--
<?php 
strtoupper("od");
strtoupper("id");
?>
--EXPECTF--
Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'strtoupper', because its argument 'str' content (id) matched the rule 'strlen array' in %a/disabled_functions_param_pos2.php on line 3