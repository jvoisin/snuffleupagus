--TEST--
Configuration line with too many quotes
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_wrong_quotes.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][error] There is an issue with the parsing of '"\)': it doesn't look like a valid string on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][error] There is an issue with the parsing of '"\)': it doesn't look like a valid string on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.