--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_mb.ini
--FILE--
<?php 
echo strlen("id");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in %a/tests/disabled_functions_mb.php:2 has been disabled.
