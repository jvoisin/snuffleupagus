--TEST--
Broken configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_cidr6.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] 'ZZZ' isn't a valid network mask. in Unknown on line 0

Fatal error: [snuffleupagus][config] 'ZZZ' isn't a valid network mask. in Unknown on line 0