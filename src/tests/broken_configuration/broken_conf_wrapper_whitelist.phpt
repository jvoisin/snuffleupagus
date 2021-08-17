--TEST--
Broken configuration with invalid token for wrapper whitelist
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_wrapper_whitelist.ini
sp.allow_broken_configuration=Off
--FILE--
<?php
echo 1337;
?>
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Unexpected keyword 'invalid_param' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Unexpected keyword 'invalid_param' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
