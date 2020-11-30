--TEST--
SESSION crypt and bad decrypt
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_crypt_session.ini
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php
// Do it like that to write (encrypt) the session and then to read (decrypt) the session
session_start();			// Start new_session , it will read an empty session
$_SESSION["toto"] = "tata"; // Encrypt and write the session
$id = session_id(); 		// Get the session_id to use it later
session_write_close(); 		// Close the session
putenv("REMOTE_ADDR=127.0.0.2");
session_id($id); 			// Recover the session with the previous session_id
session_start(); 			// Re start the session, It will read and decrypt the non empty session
var_dump($_SESSION); 		// Dump the session
?>
--EXPECTF--
Warning: [snuffleupagus][127.0.0.2][cookie_encryption][log] Something went wrong with the decryption of the session in %s/crypt_session_invalid.php on line %d
