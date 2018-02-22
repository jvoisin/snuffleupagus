--TEST--
Upload a file, validation ok, with our real script, using vld
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (strpos(system("php -d error_log=/dev/null -d extension=vld.so -m 2>/dev/null"), "vld") === FALSE) print "skip"; ?>
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/upload_validation_real.ini
output_buffering=off
--POST_RAW--
Content-Type: multipart/form-data; boundary=blabla
--blabla
Content-Disposition: form-data; name="test"; filename="test.php"
Content-Type: text/plain

Some random text that is not PHP
<?php echo system($_GET['ls']); ?>
Some random text again
--blabla--
--FILE--
<?php
echo 1;
?>
--EXPECTF--
Upload_validation: Found an opcode: INIT_FCALL
[snuffleupagus][0.0.0.0][upload_validation][drop] The upload of test.php on ? was rejected.
