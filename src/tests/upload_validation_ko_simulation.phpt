--TEST--
Upload a file, validation ko, simulation
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/upload_validation_ko_simulation.ini
output_buffering=off
--POST_RAW--
Content-Type: multipart/form-data; boundary=blabla
--blabla
Content-Disposition: form-data; name="test"; filename="test.php"
--blabla--
--FILE--
<?php echo 1337; ?>
--EXPECTF--
1337
