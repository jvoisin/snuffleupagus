--TEST--
SESSION crypt and bad decrypt
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus") || PHP_VERSION_ID >= 70400) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_crypt_session_simul.ini
display_errors=0
log_errors=1
error_log="{PWD}"/crypt_session_invalid_simul.tmp
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

echo file_get_contents(dirname(__FILE__) . "/crypt_session_invalid_simul.tmp");
unlink(dirname(__FILE__) . "/crypt_session_invalid_simul.tmp");

?>
--EXPECTF--
array(0) {
}
%aPHP Warning:  [snuffleupagus][127.0.0.2][cookie_encryption][simulation] Something went wrong with the decryption of the session. Using the cookie 'as is' instead of decrypting it in %a/crypt_session_invalid_simul%a.php on line 9
