--TEST--
Disable functions - match on argument's position
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_pos.ini
--FILE--
<?php 
system("id");
?>
--EXPECTF--
Fatal error: [snuffleupagus][config] It seems that you wrote a rule filtering on the 1337th argument of the function 'system', but it takes only 1 arguments. Matching on _all_ arguments instead. in %a/disabled_functions_param_pos.php on line 2