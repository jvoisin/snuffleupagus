--TEST--
Disable functions - exit
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_exit.ini
--FILE--
<?php 
exit('OMG');
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'exit' in %a/tests/disabled_functions_exit.php:%d has been disabled.
--XFAIL--
