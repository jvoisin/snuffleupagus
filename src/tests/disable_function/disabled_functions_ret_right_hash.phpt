--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_ret_right_hash.ini
--FILE--
<?php 
system("echo $((1 + 1336))");
?>
--EXPECTF--
1337

Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on return of the function 'system', because the function returned '1337', which matched a rule in %a/disabled_functions_ret_right_hash.php on line 2