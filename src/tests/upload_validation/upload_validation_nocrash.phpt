--TEST--
Upload validation isn't crashing
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/upload_validation_ok.ini
output_buffering=off
--FILE--
<?php
echo 1;
?>
--EXPECTF--
1
