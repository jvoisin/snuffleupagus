--TEST--
Broken configuration with non-strict mode enabled
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf.ini
sp.strict_mode=Off
--FILE--
<?php
echo 1337;
?>
--EXPECT--
PHP Fatal error:  [snuffleupagus][config] Invalid configuration prefix for 'this is a broken line' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration prefix for 'this is a broken line' on line 1 in Unknown on line 0
1337