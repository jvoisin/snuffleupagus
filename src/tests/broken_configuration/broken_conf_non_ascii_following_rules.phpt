--TEST--
Bad config, a non-ASCII rule rejects the whole ruleset (no silent ignore)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_non_ascii_following_rules.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] non-ASCII character (0xC3) in %s/config/broken_conf_non_ascii_following_rules.ini:2 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
