--TEST--
Broken configuration - ret and value are mutually exclusive
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_mutually_exclusive12.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config] Invalid configuration line: 'sp.disabled_functions.function("strcmp").drop().ret("hip").value("hop");':`ret` and `value` are mutually exclusive on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Invalid configuration line: 'sp.disabled_functions.function("strcmp").drop().ret("hip").value("hop");':`ret` and `value` are mutually exclusive on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Invalid configuration file in Unknown on line 0
Could not startup.
