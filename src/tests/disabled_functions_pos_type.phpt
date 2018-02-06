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
[snuffleupagus][0.0.0.0][config][error] It seems that you wrote a rule filtering on the 1337th argument of the function 'system', but it takes only 1 arguments. Matching on _all_ arguments instead.
[snuffleupagus][0.0.0.0][config][error] It seems that you wrote a rule filtering on the 1st argument of the function 'system', but it takes only 1 arguments. Matching on _all_ arguments instead.
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'system' in %a/tests/disabled_functions_pos_type.php:2 has been disabled, because its argument 'command' content (?) matched the rule '1'.
