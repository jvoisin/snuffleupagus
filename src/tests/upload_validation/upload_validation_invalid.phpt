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
Warning: [snuffleupagus][0.0.0.0][upload_validation] Could not call './tests/data/upload_invalid.sh' : Exec format error in Unknown on line 0
X-Powered-By: PHP/%a
Content-type: text/html; charset=UTF-8%a
%a

Fatal error: [snuffleupagus][0.0.0.0][upload_validation] The upload of test.php on ? was rejected. in Unknown on line 0
