--TEST--
Test serialize hmac
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_serialize.ini
--FILE--
<?php 
echo serialize("a");
?>
--EXPECT--
s:1:"a";650609b417904d0d9bbf1fc44a975d13ecdf6b02b715c1a06271fb3b673f25b1

