--TEST--
Disable functions - phpinfo via header_register_callback
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_extra.ini
--FILE--
<?php
header_register_callback('phpinfo');
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'phpinfo' in Unknown on line 0
