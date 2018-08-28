--TEST--
Invalid configuration file for upload validation
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/borken_conf_upload_validation.ini
--FILE--
<?php
echo 1;
?>
--EXPECTF--
PHP Fatal error:  [snuffleupagus][error] A valid string as parameter is expected on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][error] A valid string as parameter is expected on line 1 in Unknown on line 0
1