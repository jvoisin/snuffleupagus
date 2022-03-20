--TEST--
Upload a file, validation script not executable
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/upload_validation_non_exec.ini
output_buffering=off
expose_php=0
--POST_RAW--
Content-Type: multipart/form-data; boundary=blabla
--blabla
Content-Disposition: form-data; name="test"; filename="test.php"
--blabla--
--FILE--
<?php
var_dump($_FILES);
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][upload_validation][log] Could not call '%s' : Permission denied %s
%a
Fatal error: [snuffleupagus][0.0.0.0][upload_validation][drop] The upload %s was rejected. in Unknown on line 0
