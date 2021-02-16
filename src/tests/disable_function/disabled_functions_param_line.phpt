--TEST--
Disable functions - match on a specific line
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_line.ini
--FILE--
<?php 
system("echo 1337");
system("id");
?>
--EXPECTF--
1337

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'system' in %a/disabled_functions_param_line.php on line 3