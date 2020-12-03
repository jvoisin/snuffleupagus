--TEST--
Bad config, unknown keyword
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_weird_keyword.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Trailing chars '.not_a_valid_keyword("test");' at the end of '.enable().not_a_valid_keyword("test");' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Trailing chars '.not_a_valid_keyword("test");' at the end of '.enable().not_a_valid_keyword("test");' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
