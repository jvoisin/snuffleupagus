--TEST--
Invalid configuration file for upload
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/broken_conf_missing_script.ini
--FILE--
<?php
echo 1;
?>
--EXPECTF--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config] The `script` directive is mandatory in '.enable();' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] The `script` directive is mandatory in '.enable();' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config] Invalid configuration file in Unknown on line 0
Could not startup.
