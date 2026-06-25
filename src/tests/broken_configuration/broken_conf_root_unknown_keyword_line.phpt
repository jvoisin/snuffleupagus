--TEST--
Bad config, unknown root keyword reports config line
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_root_unknown_keyword_line.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Unexpected keyword 'does_not_exist' on line 5 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %a/config/broken_conf_root_unknown_keyword_line.ini on line 5
Could not startup.
