--TEST--
Invalid configuration file for upload
--SKIPIF--
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/broken_conf_missing_script.ini
--FILE--
<?php
echo 1;
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] The `script` directive is mandatory in '.upload_validation' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
