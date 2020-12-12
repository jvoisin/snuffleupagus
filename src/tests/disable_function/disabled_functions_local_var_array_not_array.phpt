--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var_array_not_array.ini
--FILE--
<?php
function foobar($a) {
  var_dump($a);
}
$a = ["lol"=>2, "test"=>1];
foobar("test");

?>
--EXPECTF--
string(4) "test"
