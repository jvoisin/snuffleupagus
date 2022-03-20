--TEST--
Invalid configuration file for readonly_exec
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/broken_conf_readonly_exec.ini
error_log=/dev/null
--FILE--
<?php
echo 1;
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Missing paranthesis for keyword 'enable' - it should be 'enable()' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
