--TEST--
Upload a file, validation ok, no simulation
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/upload_validation.ini
--POST_RAW--
Content-Type: multipart/form-data; boundary=blabla
--blabla
Content-Disposition: form-data; name="test"; filename="test.php"
--blabla--
--FILE--
<?php
echo 1;
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] A rule can't be enabled and disabled on line 1 in Unknown on line 0
