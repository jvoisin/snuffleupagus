--TEST--
Set a custom session handler, twice
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_crypt_session.ini
session.save_path = "/tmp"
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php
session_set_save_handler(new SessionHandler(), true);
session_start();
$_SESSION['a'] = 'b';
#var_dump($_SESSION);
session_destroy();
session_set_save_handler(new SessionHandler(), true);
session_start();
$_SESSION['a'] = 'b';
var_dump($_SESSION);
session_destroy();
?>
--EXPECTF--
%a
