--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_mb.ini
--FILE--
<?php 
echo strtoupper("id");
?>
--EXPECTF--
Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'strtoupper' in %a/disabled_functions_mb.php on line 2