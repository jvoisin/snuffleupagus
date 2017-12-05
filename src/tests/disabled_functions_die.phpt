--TEST--
Disable functions - die
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_die.ini
--FILE--
<?php 
die('OMG');
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'die' in %a/tests/disabled_functions_die.php:%d has been disabled.
--XFAIL--
