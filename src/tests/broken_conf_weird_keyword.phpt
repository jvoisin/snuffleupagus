--TEST--
Bad config, unknown keyword
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_weird_keyword.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] Trailing chars '.not_a_valid_keyword("test");' at the end of '.enable().not_a_valid_keyword("test");' on line 1.
