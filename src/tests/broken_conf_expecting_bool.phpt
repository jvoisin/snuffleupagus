--TEST--
Bad boolean value in configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_expecting_bool.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] Trailing chars '337);' at the end of '.enable(1337);' on line 5.
