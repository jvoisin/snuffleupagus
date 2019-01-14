--TEST--
Upload a file, validation script not executable
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/upload_validation_non_exec.ini
output_buffering=off
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
Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0

Fatal error: [snuffleupagus][config] The `script` (tests/data/upload_no_exec.sh) isn't executable on line 1 in Unknown on line 0
