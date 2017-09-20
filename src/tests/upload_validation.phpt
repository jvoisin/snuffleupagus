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
1
