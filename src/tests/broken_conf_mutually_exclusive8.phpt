--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_mutually_exclusive8.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] Invalid configuration line: 'sp.disabled_functions.ret("0").drop();': must take a function name on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration line: 'sp.disabled_functions.ret("0").drop();': must take a function name on line 1 in Unknown on line 0