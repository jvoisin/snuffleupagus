--TEST--
Disable functions - shell_exec via register_shutdown_function
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_extra.ini
--FILE--
<?php
register_shutdown_function('shell_exec', 'ls');
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'shell_exec', %a matched a rule in Unknown on line 0
