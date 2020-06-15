--TEST--
Upload a file, validation ko, no simulation
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/upload_validation_ko.ini
output_buffering=off
--POST_RAW--
Content-Type: multipart/form-data; boundary=blabla
--blabla
Content-Disposition: form-data; name="test"; filename="test.php"
--blabla--
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][upload_validation] The upload of test.php on ? was rejected. in Unknown on line 0