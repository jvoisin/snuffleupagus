--TEST--
SESSION ID too short
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/sid_length_limit.ini
--FILE--
<?php
session_id('123');
session_start();
echo strlen(session_id());
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][session][drop] Session ID is too short in %a.php on line %d

Fatal error: [snuffleupagus][0.0.0.0][session][drop] Session ID is too short in Unknown on line 0