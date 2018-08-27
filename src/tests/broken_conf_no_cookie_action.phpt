--TEST--
Bad config, invalid action.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_cookie_action.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] You must specify a at least one action to a cookie on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] You must specify a at least one action to a cookie on line 1 in Unknown on line 0