--TEST--
Broken configuration with allow broken turned off
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf.ini
sp.allow_broken_configuration=Off
--FILE--
<?php
echo 1337;
?>
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Invalid configuration prefix for 'this is a broken line' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration prefix for 'this is a broken line' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
