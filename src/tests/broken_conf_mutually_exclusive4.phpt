--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_mutually_exclusive4.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] Invalid configuration line: 'sp.disabled_functions.function("system").param("id").value("42").param_r("^id$").drop();':'.r_param' and '.param' are mutually exclusive.