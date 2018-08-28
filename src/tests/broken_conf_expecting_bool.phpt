--TEST--
Bad boolean value in configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_expecting_bool.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] Trailing chars '337);' at the end of '.enable(1337);' on line 5 in Unknown on line 0

Fatal error: [snuffleupagus][config] Trailing chars '337);' at the end of '.enable(1337);' on line 5 in Unknown on line 0