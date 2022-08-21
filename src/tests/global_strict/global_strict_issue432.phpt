--TEST--
Global strict mode, for issue #432
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/global_strict_disabled.ini
--FILE--
<?php 
$filename = '/tmp/test.txt';
file_put_contents($filename, '0');
$var = file_get_contents($filename);
if ($var == "0") {
	print("WIN");
}
if ($var == 0) {
	print("WIN");
}
unlink($filename);
?>
--EXPECT--
WINWIN
