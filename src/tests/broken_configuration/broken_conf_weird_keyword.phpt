--TEST--
Bad config, unknown keyword
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_weird_keyword.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config] Trailing chars '.not_a_valid_keyword("test");' at the end of '.enable().not_a_valid_keyword("test");' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Trailing chars '.not_a_valid_keyword("test");' at the end of '.enable().not_a_valid_keyword("test");' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Invalid configuration file in Unknown on line 0
Could not startup.