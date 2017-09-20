--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_mutually_exclusive5.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] Invalid configuration line: 'sp.disabled_functions.function("system").ret("0").drop().ret_r("^0$");':'.r_ret' and '.ret' are mutually exclusive.