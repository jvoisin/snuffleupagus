--TEST--
Bad config, invalid action.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_cookie_action.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][config][error] You must specify a at least one action to a cookie on line 1.
