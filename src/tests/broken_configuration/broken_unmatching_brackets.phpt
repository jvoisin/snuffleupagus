--TEST--
Broken configuration - unmatching brackets
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_unmatching_brackets.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid `]` position. in %s/broken_configuration/config/config_unmatching_brackets.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid value 'arr[b]]]]]' for `param` on line 1 in %s/broken_configuration/config/config_unmatching_brackets.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/config_unmatching_brackets.ini on line 1
Could not startup.
