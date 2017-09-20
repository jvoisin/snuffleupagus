--TEST--
Configuration line with an empty string
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_line_empty_string.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][error][error] There is an issue with the parsing of '': it doesn't look like a valid string.
