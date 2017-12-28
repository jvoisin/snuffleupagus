--TEST--
Shipped configuration
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../config/examples.ini
--FILE--
<?php 
ob_start();
phpinfo();
$info = ob_get_clean();
ob_get_clean();
if (strstr($info, 'Valid config => yes') !== FALSE) {
	echo "win";
} else {
	echo "lose";
}
?>
--EXPECTF--
win
