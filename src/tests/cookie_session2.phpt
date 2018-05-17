--TEST--
PHPSESSID session id not too long
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_session_cookie.ini
--COOKIE--
PHPSESSID=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICY5m6bfe71ThWA3CCvYOvwAKMaG/M8kS1hGgJpJYVrWQ==;
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php
session_id("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICY5m6bfe71ThWA3CCvYOvwAKMaG/M8kS1hGgJpJYVrWQ==");
session_start();
var_dump($_SESSION); 		// Dump the session
?>
--EXPECTF--
array(1) {
  ["test"]=>
  string(4) "tata"
}