--TEST--
Disable functions - exec
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_extra.ini
--FILE--
<?php 
echo exec('ls -l');
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'exec' in %a.php on line 2
