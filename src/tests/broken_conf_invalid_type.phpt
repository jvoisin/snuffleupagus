--TEST--
Broken conf with wrong type
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_type.ini
--FILE--
--EXPECTF--
[snuffleupagus][0.0.0.0][error][error] There is an issue with the parsing of '"totally_wrong"_type")': it doesn't look like a valid string on line 1.
