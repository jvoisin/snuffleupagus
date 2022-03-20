--TEST--
Broken configuration with allow broken turned off
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf.ini
sp.allow_broken_configuration=Off
error_log=/dev/null
--FILE--
<?php
echo 1337;
?>
--EXPECT--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Parser error on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
