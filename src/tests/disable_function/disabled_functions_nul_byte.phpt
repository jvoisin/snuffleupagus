--TEST--
Disable functions with nul byte
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_nul_byte.ini
--FILE--
<?php 
system("\0id");
system("id");

?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'system', because its argument '$command' content (%s0id) matched a rule in %a/disabled_functions_nul_byte.php on line 2
