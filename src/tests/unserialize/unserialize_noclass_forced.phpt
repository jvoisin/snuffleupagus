--TEST--
Unserialize with noclass forced
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus") || PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_serialize_noclass.ini
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
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][unserialize_noclass][drop] unserialize_noclass is only supported on PHP8+ in %s/tests/unserialize/unserialize_noclass_forced.php on line 9
