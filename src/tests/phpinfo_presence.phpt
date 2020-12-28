--TEST--
Unserialize fail
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
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
