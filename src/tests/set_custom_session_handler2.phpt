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
Warning: SessionHandler::open(): Cannot call session save handler in a recursive manner in%s/tests/set_custom_session_handler2.php on line 8

Warning: SessionHandler::close(): Cannot call session save handler in a recursive manner in %stests/set_custom_session_handler2.php on line 8

Warning: session_start(): Failed to initialize storage module: user (path: %s) in %stests/set_custom_session_handler2.php on line 8
array(1) {
  ["a"]=>
  string(1) "b"
}

Warning: session_destroy(): Trying to destroy uninitialized session in %s/tests/set_custom_session_handler2.php on line %d
