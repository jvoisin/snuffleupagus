--TEST--
Non-ASCII character in the rules file
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_non_ascii_char.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][%s][config][log] Invalid configuration file in Unknown on line 0

Fatal error: [snuffleupagus][%s][config][log] Non-ASCII character (0xC3) in %a/config/broken_conf_non_ascii_char.ini:1 in Unknown on line 0
