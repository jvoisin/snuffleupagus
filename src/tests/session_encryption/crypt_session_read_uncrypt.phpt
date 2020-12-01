--TEST--
SESSION crypt/decrypt valid
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_crypt_session_simul.ini
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php
$current_path = dirname(getcwd()) . "/src/tests/" ;
ini_set("session.save_path", $current_path);

session_start();
$id = session_id(); 											// Get the session_id to use it later
$filename_sess = $current_path . "sess_" . $id;
file_put_contents($filename_sess, "toto|s:4:\"tata\";"); 			// Write a unencrypted session
session_write_close(); 											// Close the session

session_id($id);
session_start();												// Try to read the unencrypted session, it will fail to decrypt but it must return the session
var_dump($_SESSION);
echo "OK";
unlink($filename_sess);
?>
--EXPECTF--
array(1) {
  ["toto"]=>
  string(4) "tata"
}
OK
