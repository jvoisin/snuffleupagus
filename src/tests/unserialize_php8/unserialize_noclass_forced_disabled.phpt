--TEST--
Unserialize with noclass forced disabled
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_serialize_noclass_disabled.ini
--FILE--
<?php 
class C {
   public $name;
}
$c = new C;
$c->name = "test";

$a = serialize($c);
var_dump(unserialize($a, ['allowed_classes' => false]));
var_dump(unserialize($a, ['allowed_classes' => true ]));
var_dump(unserialize($a));
?>
--EXPECT--
object(__PHP_Incomplete_Class)#2 (2) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(1) "C"
  ["name"]=>
  string(4) "test"
}
object(C)#2 (1) {
  ["name"]=>
  string(4) "test"
}
object(C)#2 (1) {
  ["name"]=>
  string(4) "test"
}

