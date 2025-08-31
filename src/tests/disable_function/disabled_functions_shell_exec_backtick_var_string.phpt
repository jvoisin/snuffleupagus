--TEST--
Disable functions - shell_exec via backtick operator in context of a var name in a string
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_extra.ini
error_reporting = E_ALL & ~E_DEPRECATED
--FILE--
<?php 
echo "{${`ls`}}";
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'shell_exec', %a matched a rule in %a.php on line 2
