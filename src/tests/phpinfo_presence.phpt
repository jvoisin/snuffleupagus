--TEST--
Unserialize fail
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../config/default.rules
--FILE--
<?php 
ob_start () ;
phpinfo () ;
$pinfo = ob_get_contents () ;
ob_end_clean () ;
if (strstr($pinfo, "snuffleupagus") !== FALSE) 
	echo 1;
else
	echo 2;
?>
--EXPECT--
1
