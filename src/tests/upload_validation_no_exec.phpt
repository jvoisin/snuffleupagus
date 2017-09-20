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
echo "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
?>
--EXPECTF--
array(1) {
  ["test"]=>
  array(5) {
    ["name"]=>
    string(8) "test.php"
    ["type"]=>
    string(0) ""
    ["tmp_name"]=>
    string(0) ""
    ["error"]=>
    int(3)
    ["size"]=>
    int(0)
  }
}
