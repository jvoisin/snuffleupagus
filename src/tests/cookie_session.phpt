--TEST--
PHPSESSID session id not too long
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_session_cookie.ini
--FILE--
<?php

// Do it like that to write to the session and then to read the session
// Start new_session
session_start();
$_SESSION["toto"] = "tata";
$id = session_id(); // Get the session_id to use it later
session_write_close(); // Close the session
session_id($id); // Recover the session with the previous session_id
session_start(); // Re start the session
//var_dump($_SESSION); // Dump the session

?>
--EXPECTF--
abcdfdadas