--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_mutually_exclusive6.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Invalid configuration line: 'sp.disabled_functions.function("system").param("id").value("42").ret_r("^0$").drop();':`ret` and `param` are mutually exclusive on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration line: 'sp.disabled_functions.function("system").param("id").value("42").ret_r("^0$").drop();':`ret` and `param` are mutually exclusive on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.