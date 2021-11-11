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
s:1:"a";cdbc93e593656164d448db33e4668a3f30fa794d6658016365f7eb453d48b022

