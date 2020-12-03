--TEST--
Unserialize ok, but called with the wrong number of arguments
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_serialize.ini
--FILE--
<?php 
$a=serialize("a");
var_dump(unserialize($a, "too", "many", "aaaaaaaarguments!"));
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: unserialize() expects at most 2 arguments, 4 given in %s/tests/unserialize/unserialize_wrong_call.php:3
Stack trace:
#0 %s/tests/unserialize/unserialize_wrong_call.php(3): unserialize('s:1:"a";650609b...', 'too', 'many', 'aaaaaaaargument...')
#1 {main}

