--TEST--
Set a custom session handler
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_crypt_session.ini
session.save_handler = 
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php 
echo "win";
?>
--EXPECT--
win
