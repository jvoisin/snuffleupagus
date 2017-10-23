--TEST--
Upload a file, invalid validation script
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/upload_validation_invalid.ini
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
[snuffleupagus][0.0.0.0][upload_validation][error] Could not call './tests/data/upload_invalid.sh' : Exec format error
[snuffleupagus][0.0.0.0][upload_validation][drop] The upload of test.php on ? was rejected.
