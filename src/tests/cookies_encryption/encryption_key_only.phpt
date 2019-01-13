--TEST--
Encryption key only
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/encryption_key_only.ini
--FILE--
<?php
echo 1337;
?>
--EXPECT--
1337

