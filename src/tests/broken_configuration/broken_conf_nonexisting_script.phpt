--TEST--
Invalid configuration file for upload
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/broken_conf_nonexisting_script.ini
error_log=/dev/null
--FILE--
<?php
echo 1;
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] The `script` (./non_existing_script.sh) doesn't exist on line 1 in %s/broken_configuration/config/broken_conf_nonexisting_script.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_nonexisting_script.ini on line 1
Could not startup.
