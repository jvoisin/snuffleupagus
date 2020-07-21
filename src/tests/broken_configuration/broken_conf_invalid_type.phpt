--TEST--
Broken conf with wrong type
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_type.ini
--FILE--
--EXPECTF--
PHP Fatal error:  [snuffleupagus][0.0.0.0][error][log] There is an issue with the parsing of '"totally_wrong"_type")': it doesn't look like a valid string on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][error][log] There is an issue with the parsing of '"totally_wrong"_type")': it doesn't look like a valid string on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.