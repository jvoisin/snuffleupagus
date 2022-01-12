--TEST--
SESSION crypt/decrypt valid
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (!extension_loaded("session")) print "skip"; ?>
--EXTENSIONS--
session
--INI--
sp.configuration_file={PWD}/config/config_crypt_session_simul.ini
session.save_path="{PWD}"
display_errors=0
log_errors=1
error_log="{PWD}"/crypt_session_read_uncrypt.tmp
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php
session_start();
$id = session_id(); 											// Get the session_id to use it later
$filename_sess = dirname(__FILE__) . "/sess_" . $id;
file_put_contents($filename_sess, "toto|s:4:\"tata\";"); 			// Write unencrypted session
session_write_close(); 											// Close the session

session_id($id);
session_start();												// Try to read the unencrypted session, it will fail to decrypt but it must return the session
var_dump($_SESSION);
echo "OK\n";
echo file_get_contents(dirname(__FILE__) . "/crypt_session_read_uncrypt.tmp");
unlink(dirname(__FILE__) . "/crypt_session_read_uncrypt.tmp");
unlink($filename_sess);
?>
--EXPECTF--
array(1) {
  ["toto"]=>
  string(4) "tata"
}
OK
%aPHP Warning:  [snuffleupagus][127.0.0.1][cookie_encryption][simulation] Buffer underflow tentative detected in cookie encryption handling for the session. Using the cookie 'as is' instead of decrypting it in %a/crypt_session_read_uncrypt.php on line 9
