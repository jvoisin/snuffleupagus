--TEST--
Disable functions - match on argument's position
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_pos.ini
--FILE--
<?php
system([123, 456]);
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'system', because its argument 'command' content (?) matched the rule '1' in %a/disabled_functions_pos_type.php on line %d
