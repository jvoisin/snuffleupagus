--TEST--
Check the syslog output, in simulation mode.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/syslog_simulation.ini
--FILE--
<?php 
var_dump(unserialize('s:1:"a";'));
?>
--EXPECTF--
