--TEST--
PHPSESSID session id not too long
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_session_cookie.ini
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php
// Do it like that to write to the session and then to read the session
session_start();			// Start new_session , it will read an empty session
$_SESSION["toto"] = "tata"; // Write to the session
$id = session_id(); 		// Get the session_id to use it later
session_write_close(); 		// Close the session
session_id($id); 			// Recover the session with the previous session_id
session_start(); 			// Re start the session, It will read the non empty session
var_dump($_SESSION); 		// Dump the session
?>
--EXPECTF--
array(1) {
  ["toto"]=>
  string(4) "tata"
}
