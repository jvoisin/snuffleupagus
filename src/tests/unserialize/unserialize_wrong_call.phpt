--TEST--
Unserialize ok, but called with the wrong number of arguments
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_serialize.ini
--FILE--
<?php 
$a=serialize("a");
var_dump(unserialize($a, "too", "many", "aaaaaaaarguments!"));
?>
--EXPECTF--
Warning: unserialize() expects at most 2 parameters, 4 given in %a/unserialize_wrong_call.php on line %d
NULL
