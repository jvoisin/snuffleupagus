--TEST--
SESSION ID too long
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (!extension_loaded("session")) print "skip"; ?>
--EXTENSIONS--
session
--INI--
sp.configuration_file={PWD}/config/sid_length_limit.ini
--FILE--
<?php
session_id('123456789012345678901234567890123');
session_start();
echo strlen(session_id());
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][session][drop] Session ID is too long in %a.php on line %d

Fatal error: [snuffleupagus][0.0.0.0][session][drop] Session ID is too long in Unknown on line 0