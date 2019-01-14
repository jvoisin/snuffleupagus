--TEST--
Unserialize ok
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_serialize.ini
--FILE--
<?php 
$a=serialize("a");
var_dump(unserialize($a));
?>
--EXPECT--
string(1) "a"
