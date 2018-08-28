--TEST--
Disable functions - match on argument's position
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_pos.ini
--FILE--
<?php
system([123, 456]);
?>
--EXPECTF--
Warning: [snuffleupagus][config] It seems that you wrote a rule filtering on the 1337th argument of the function 'system', but it takes only 1 arguments. Matching on _all_ arguments instead. in %a/tests/disabled_functions_pos_type.php on line %d

Warning: [snuffleupagus][config] It seems that you wrote a rule filtering on the 1st argument of the function 'system', but it takes only 1 arguments. Matching on _all_ arguments instead. in %a/tests/disabled_functions_pos_type.php on line %d

Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'system', because its argument 'command' content (?) matched the rule '1' in %a/tests/disabled_functions_pos_type.php on line %d
