--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_mutually_exclusive2.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] Invalid configuration line: 'sp.disabled_functions.function("system").function_r("system").param("id").value("42").drop();': '.r_function' and '.function' are mutually exclusive.