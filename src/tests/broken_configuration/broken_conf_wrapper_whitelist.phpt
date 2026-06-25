--TEST--
Broken configuration with invalid token for wrapper whitelist
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_wrapper_whitelist.ini
sp.allow_broken_configuration=Off
error_log=/dev/null
--FILE--
<?php
echo 1337;
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Unexpected keyword 'invalid_param' in %a/config/broken_conf_wrapper_whitelist.ini on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %a/config/broken_conf_wrapper_whitelist.ini on line 1
Could not startup.
