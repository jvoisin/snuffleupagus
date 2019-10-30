--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_mutually_exclusive3.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config] Invalid configuration line: 'sp.disabled_functions.function("system").param("id").value("42").filename_r("^id$").filename("pouet.txt").drop();': '.r_filename' and '.filename' are mutually exclusive on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Invalid configuration line: 'sp.disabled_functions.function("system").param("id").value("42").filename_r("^id$").filename("pouet.txt").drop();': '.r_filename' and '.filename' are mutually exclusive on line 1 in Unknown on line 0