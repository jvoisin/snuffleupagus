--TEST--
Disable functions - shell_exec via filter_input callback
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_extra.ini
--GET--
cmd=ls
--FILE--
<?php
echo filter_input(INPUT_GET, 'cmd', FILTER_CALLBACK, array('options' => 'shell_exec'));
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'shell_exec', %a matched a rule in %a.php on line 2
