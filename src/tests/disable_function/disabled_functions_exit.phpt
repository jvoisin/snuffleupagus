--TEST--
Disable functions - exit
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_exit.ini
--FILE--
<?php 
exit('OMG');
?>
--XFAIL--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'exit' in %a/disabled_function_echo.php on line 3
