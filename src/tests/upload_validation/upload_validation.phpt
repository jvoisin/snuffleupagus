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
Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/upload_validation/config/upload_validation.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] The `script` (tests/upload_ko.sh) doesn't exist on line 1 in %s/upload_validation/config/upload_validation.ini on line 1
