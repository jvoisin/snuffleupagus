--TEST--
Invalid configuration file for upload validation
--SKIPIF--
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/broken_conf_upload_validation.ini
--FILE--
<?php
echo 1;
?>
--EXPECTF--
PHP Fatal error:  [snuffleupagus][0.0.0.0][error][log] A valid string as parameter is expected on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][error][log] A valid string as parameter is expected on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
